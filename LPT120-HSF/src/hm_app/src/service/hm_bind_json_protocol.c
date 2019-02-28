
#include "hsf.h"
#include "cJSON.h"
#include "hm_parameter.h"
#include "hm_system.h"
#include "hm_connect_cloud_handle.h"
#include "hm_uart_handle.h"
#include "hm_bind_json_protocol.h"
#include "hm_dev_model.h"
#include "hm_MD5_sum.h"
#define TABLE_FINAL_MASK	0
#define ACTION_CODE_ITEM_FINAL {TABLE_FINAL_MASK, ""}

#define CODE_DESC_LENGTH_MAX	16

struct action_code_item
{
	uint32_t code;
	char desc[CODE_DESC_LENGTH_MAX];
};


const struct action_code_item g_action_code_table[] = {
	{ ACTION_CODE_SUCCESS ,					"succuss"},
	{ ACTION_CODE_ERROR_PARSE_ERROR ,		"parse error" },
	{ ACTION_CODE_ERROR_ACTION_UNKNOWN ,	"action unknown" },
	{ ACTION_CODE_ERROR_UART_TIMEOUT ,		"uart timeout" },
	{ ACTION_CODE_ERROR_FORMAT ,			"format error" },
	{ ACTION_CODE_ERROR_DEVTID ,			"devTid error" },
	{ ACTION_CODE_ERROR_CTRLKEY ,			"ctrlKey error" },
	{ ACTION_CODE_ERROR_EXECUTION ,			"execute error" },
	{ ACTION_CODE_ERROR_BIN_TYPE ,			"bin type error" },
	{ ACTION_CODE_ERROR_UPDATE_STARTED ,	"update started" },
	{ ACTION_CODE_ERROR_FRAME_INVALID ,		"frame invalid" },
	{ ACTION_CODE_ERROR_INTERNAL_ERROR ,	"internal error" },
	{ ACTION_CODE_ERROR_NO_LAN_USER ,		"user not exist" },
	{ ACTION_CODE_ERROR_LAN_USER_FULL ,		"users full" },
	{ ACTION_CODE_ERROR_VALUE_INVALID ,		"value invalid" },
	{ ACTION_CODE_ERROR_MCUUPGRADE_BUSY ,	"mcu update busy" },
	ACTION_CODE_ITEM_FINAL
};

// extern void hm_48_repond_data_error_frame(void);
extern int hm_48_repond_ret_frame(uint8_t *buf, uint32_t len);

//need hfmem_free
 
char *get_code_desc(uint32_t code)
{
	const struct action_code_item *item_cur = g_action_code_table;
	struct action_code_item item;
	char *desc = NULL;

	do
	{
		memcpy(&item, item_cur, sizeof(struct action_code_item));
		if (item.code == TABLE_FINAL_MASK) { goto fail; }
		if (item.code == code)
		{
			uint32_t desc_len = strlen(item.desc);
			desc = (char *)hfmem_malloc(desc_len + 1);
			if (desc == NULL) { goto fail; }
			memcpy(desc, item.desc, desc_len);
			desc[desc_len] = '\0';
			goto done;
		}
		item_cur++;
	} while (1);


	goto done;
done:
fail:
	return desc;
}

 
void action_resp_handle(cJSON *msgid, cJSON *action, cJSON *params, uint32_t code)
{
	char *ret_str = NULL, *desc = NULL;
	cJSON *ret = NULL;
	char action_resp[JSON_ACTION_RESP_LENGTH_MAX] = { 0 };

	ASSERT((msgid == NULL) || (action == NULL));

	ret = cJSON_CreateObject();
	ASSERT(ret == NULL);
	sprintf(action_resp, "%s"RESP"", action->valuestring);
	desc = get_code_desc(code);
	cJSON_AddNumberToObject(ret, JSON_PROTOCOL_FIELD_MSGID, msgid->valueint);
	cJSON_AddStringToObject(ret, JSON_PROTOCOL_FIELD_ACTION, action_resp);
	cJSON_AddNumberToObject(ret, JSON_PROTOCOL_FIELD_CODE, code);
	cJSON_AddStringToObject(ret, JSON_PROTOCOL_FIELD_DESC, desc ? desc : "");
	if (params)
	{
		cJSON_AddItemToObject(ret, JSON_PROTOCOL_FIELD_PARAMS, params);
		ret_str = cJSON_PrintUnformatted(ret);
		cJSON_DetachItemFromObject(ret, "params");
	}
	else
	{
		ret_str = cJSON_PrintUnformatted(ret);
	}

	ASSERT(ret_str == NULL);
	hm_tcp_send_data(ret_str, strlen(ret_str));

	goto done;

done:
fail:
	if (ret) { cJSON_Delete(ret); }
	if (ret_str) { hfmem_free(ret_str); }
	if (desc) { hfmem_free(desc); }
	return;
}

static void bind_get_prodinfo_resp(cJSON *whole)
{
	ASSERT(whole == NULL);
	cJSON *request = whole;

	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *params = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_PARAMS);
	cJSON *code = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_CODE);
	//返回信息错误
	if (code && code->valueint != 200)
	{
		goto fail;
	}

	if (action && params && strncmp(action->valuestring, "getProdInfoResp", sizeof("getProdInfoResp")) == 0)
	{
		cJSON *mid = cJSON_GetObjectItem(params, "mid");
		cJSON *workMode = cJSON_GetObjectItem(params, "workMode");
		cJSON *serviceHost = cJSON_GetObjectItem(params, "serviceHost");
		cJSON *servicePort = cJSON_GetObjectItem(params, "servicePort");
		cJSON *encryptType = cJSON_GetObjectItem(params, "encryptType");
		cJSON *tokenType = cJSON_GetObjectItem(params, "tokenType");
		u_printf("HM serviceHost  = %s\n", serviceHost->valuestring);
		if (mid && workMode && serviceHost && servicePort && encryptType)
		{
			ASSERT(product_info_set_mid(mid->valuestring) !=0);
			ASSERT(product_info_set_work_mode(workMode->valueint) !=0);
			ASSERT(product_info_set_service_hostname(serviceHost->valuestring) != 0);
			ASSERT(product_info_set_service_port(servicePort->valueint) !=0);
			ASSERT(product_info_set_token_type(tokenType->valueint) != 0);
			product_info_set_encrypt_type(ENCRYPT_TYPE_NONE);
		}
		else
		{
			goto fail;
		}
	}
	else
	{
		//参数错误
		goto fail;
	}
	//参数正确，发送相关消息到任务。
	hm_sys_run_info.run_info.cc_step_status = CC_RECEIVED_DEVICE_INFO;
	process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
    return ;

fail:
	hm_sys_run_info.run_info.cc_step_status = CC_START;
	process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
	return ;
	
}

static void bind_request_verify_resp(cJSON *whole)
{
	ASSERT(whole == NULL);
	cJSON *request = whole;

	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *params = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_PARAMS);
	cJSON *code = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_CODE);
	//返回信息错误
	if (code && code->valueint != 200)
	{
		//添加相关处理
		goto fail;
	}
	ASSERT((msgid == NULL) || (action == NULL) || (params == NULL));

	cJSON *randomkey = cJSON_GetObjectItem(params, "randomKey");
	ASSERT((randomkey == NULL));

	//根据收到的randomkey计算出authkey
	 //MD5(randomKey + devTid + devPriKey);
	//product_info_set_authkey(const char *authkey);
		//参数正确，发送相关消息到任务。
	//添加相关处理 发送认证消息
	char *srandomkey = randomkey->valuestring;
	char *stid = product_info_get_devtid();
	char *sdevprikey = product_info_get_devprikey ();
	char sauthkey[AUTHORIZATION_LENGTH_MAX +1];

	if( hm_make_authkey(sauthkey, srandomkey, stid, sdevprikey) == 0)
	{
		u_printf("HM AUTHKEY MAKE SUCCESSS");
		product_info_set_authkey(sauthkey);
		gatewaylogin_report();
	}
	else
	{
		u_printf("HM AUTHKEY MAKE FAIL");
		goto fail;
	}
	return ;
	

fail :
	hm_sys_run_info.run_info.cc_step_status = CC_START;
	process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
	return;
}

static void bind_gateway_login_resp(cJSON *whole)
{
	ASSERT(whole == NULL);
	cJSON *request = whole;

	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *params = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_PARAMS);
	cJSON *code = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_CODE);
	//返回信息错误
	if (code && code->valueint != 200)
	{
		//添加相关处理
		goto fail;
	}
	ASSERT((msgid == NULL) || (action == NULL) || (params == NULL));

	cJSON *ctrlkey = cJSON_GetObjectItem(params, "ctrlKey");
	ASSERT((ctrlkey == NULL));

	product_info_set_ctrlkey(ctrlkey->valuestring);
	//参数正确，发送相关消息到任务。
	//添加相关处理 发送认证消息
	reportdevinfo_report();
	return ;

fail:
	hm_sys_run_info.run_info.cc_step_status = CC_START;
	process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
	return;
}

static void bind_report_devinfo_resp(cJSON *whole)
{
	ASSERT(whole == NULL);
	cJSON *request = whole;

	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *code = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_CODE);
	//返回信息错误
	if (code && code->valueint != 200)
	{
		//添加相关处理
		//网关登入失败
		goto fail;
	}
	ASSERT((msgid == NULL) || (action == NULL));

	//网关登入成功
	//参数正确，发送相关消息到任务。
	//添加相关处理 发送认证消息
	hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_LOGIN_SUCCESS;
	process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
    return ;
fail:
	hm_sys_run_info.run_info.cc_step_status = CC_START;
	process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
	return;
}
static void bind_gateway_bind_resp(cJSON *whole)
{
	ASSERT(whole == NULL);
	cJSON *request = whole;

	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *code = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_CODE);
	//返回信息错误
	if (code && code->valueint != 200)
	{
		//添加相关处理
		//网关绑定失败
		hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_BIND_FAIL;
		process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
		goto fail;
	}
	ASSERT((msgid == NULL) || (action == NULL));

	//网关绑定成功
	//参数正确，发送相关消息到任务。
	//添加相关处理 发送认证消息
	hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_BIND_SUCCESS;
	process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
    return;
fail:
	
	return;

}

static void bind_add_subdev_resp(cJSON *whole)
{
	ASSERT(whole == NULL);
	cJSON *request = whole;

	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *code = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_CODE);
	//返回信息错误
	if (code && code->valueint != 200)
	{
		//添加相关处理
		//子设备添加失败
		goto fail;
	}
	ASSERT((msgid == NULL) || (action == NULL));

	//子设备添加成功
	//参数正确，发送相关消息到任务。
	//添加相关处理 发送认证消息

fail:

	return;
}

static void bind_report_subdevinfo_resp(cJSON *whole)
{
	ASSERT(whole == NULL);
	cJSON *request = whole;

	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *code = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_CODE);
	//返回信息错误
	if (code && code->valueint != 200)
	{
		//添加相关处理
		//子设备上报状态失败
		goto fail;
	}
	ASSERT((msgid == NULL) || (action == NULL));

	//子设备上报状态成功
	//参数正确，发送相关消息到任务。
	//添加相关处理 发送认证消息

fail:

	return;
}


static void bind_appsend(cJSON *whole)
{
	#define MAX_TOTAL_ATTRS_LENTH  255
	//u_printf("bind_appsend\n\n");
	uint32_t code = ACTION_CODE_SUCCESS;
	char *ctrlkey_local = NULL;
	cJSON *request = whole;
	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *params = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_PARAMS);
	ASSERT_EVAL((msgid == NULL) || (action == NULL) || (params == NULL),
		code, ACTION_CODE_ERROR_FORMAT);

	cJSON *devtid = cJSON_GetObjectItem(params, "devTid");
	cJSON *ctrlkey = cJSON_GetObjectItem(params, "ctrlKey");
	cJSON *apptid = cJSON_GetObjectItem(params, "appTid");
	cJSON *subdevtid = cJSON_GetObjectItem(params, "subDevTid");
	cJSON *data = cJSON_GetObjectItem(params, "data");
	ASSERT_EVAL((devtid == NULL) || (ctrlkey == NULL) || (apptid == NULL) || (data == NULL),
		code, ACTION_CODE_ERROR_FORMAT);

	/*match tid*/
	char *dev_tid = product_info_get_devtid();
	ASSERT_EVAL((strncmp(dev_tid, devtid->valuestring, DEVTID_LENGTH_MAX) != 0),
				code, ACTION_CODE_ERROR_DEVTID);

	/*match ctrlkey*/
	ctrlkey_local = product_info_get_ctrlkey();
	ASSERT_EVAL((ctrlkey_local == NULL) || (strcmp(ctrlkey_local, ctrlkey->valuestring) != 0),
		code, ACTION_CODE_ERROR_CTRLKEY);

	if (subdevtid->valuestring == NULL)
	{
		//发送给网关本身的消息
		//添加设备
		cJSON *subdev_mid = cJSON_GetObjectItem(data, "subMid");
		cJSON *overtime = cJSON_GetObjectItem(data, "overtime");
		ASSERT_EVAL((subdev_mid == NULL) || (overtime == NULL),
					code, ACTION_CODE_ERROR_FORMAT);

		//串口发送添加设备的命令
		mid_t tmid;
		tmid.len = strlen(subdev_mid->valuestring);
		//char *subdev_mid_value = str_to_hex(subdev_mid->valuestring, strlen(subdev_mid->valuestring)); //need free
		memcpy(tmid.data, subdev_mid->valuestring, tmid.len);
		hm_48_add_subdev_frame_send(&tmid);
		goto done;
	}
	else
	{
		char *subdev_mac = str_to_hex(subdevtid->valuestring, strlen(subdevtid->valuestring)); //need free
		tid_t ttid;
		cJSON *cmdid = cJSON_GetObjectItem(data, "cmdId");
		uint8 cmd_value = cmdid->valueint;
		uint8 i = 0;
		cJSON *attr = NULL;
		char *pattrs = hfmem_malloc(MAX_TOTAL_ATTRS_LENTH) ;
		uint8 attrs_bytes_num = 0;
		ttid.len = strlen(subdevtid->valuestring) / 2 ;
		memcpy(ttid.data, subdev_mac, ttid.len);
		char *dev_type_name = hm_get_dev_type(&ttid); //根据设备ID获取设备类型
		cmd_info_t *tcmd_info = hm_check_cmd_id(dev_type_name, cmd_value); //根据设备类型获取命令下的属性列表
		for (i = 0; i < tcmd_info->attr_num ; i++)
		{
			char *tattr_name = hm_get_attr_name(WT_DEV_TYPE_NAME, tcmd_info->attr_list[i]); //根据设备类型获取属性名称
			attr = cJSON_GetObjectItem(data, tattr_name);
			if (attr == NULL)
			{
				continue;
			}
			switch (attr->type)
			{
				case cJSON_String:
				{
					memcpy(pattrs, &(attr->valuestring), sizeof(attr->valuestring));
					pattrs = pattrs + sizeof(attr->valuestring);
					attrs_bytes_num = attrs_bytes_num + sizeof(attr->valuestring);
					break;
				}
				case cJSON_Number:
				{
					memcpy(pattrs, &(attr->valueint), sizeof(attr->valueint));
					pattrs = pattrs + sizeof(attr->valueint);
					attrs_bytes_num = attrs_bytes_num + sizeof(attr->valueint);
					break;
				}
				default :
				{
					break ;
				}
			}				
		}
		//通过串口发送出去
		hm_48_valid_data_t tvalid_data;
		tvalid_data.dev_type = dev_type_name;
		tvalid_data.tid = ttid;
		tvalid_data.cmd_id = cmd_value;
		tvalid_data.attrs_lenth = attrs_bytes_num;
		tvalid_data.attrs_data = pattrs;
		hm_48_app_to_subdev_frame_send(&tvalid_data);
		__FREE(subdev_mac);
		__FREE(pattrs);

	}

	uint32_t msgid_tmp = msgid->valueint;
	char *apptid_tmp = apptid->valuestring;

//相关处理
	goto done;
done:
	action_resp_handle(msgid, action, params, code);
	if (code == ACTION_CODE_SUCCESS)
	{
		
	}
	return 0;

fail:
	action_resp_handle(msgid, action, params, code);
	
	return -1;
}


static void bind_dev_send_resp(cJSON *whole)
{
	cJSON *respose = whole;
	cJSON *code = cJSON_GetObjectItem(respose, "code");
	ASSERT(code == NULL);
	if (code->valueint != 200)
	{
		//处理相关
		hm_48_repond_data_error_frame();
	
	}


fail:
	return;
}

static void  bind_dev_delete(cJSON *whole)
{
	//debug_print("bind_appsend\n\n");
	uint32_t code = ACTION_CODE_SUCCESS;
	char *ctrlkey_local = NULL, *dev_tid = NULL,*frame_str = NULL;
	cJSON *request = whole;
	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *params = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_PARAMS);
	ASSERT_EVAL((msgid == NULL) || (action == NULL) || (params == NULL),
				code, ACTION_CODE_ERROR_FORMAT);

	cJSON *devtid = cJSON_GetObjectItem(params, "devTid");
	cJSON *ctrlkey = cJSON_GetObjectItem(params, "ctrlKey");
	cJSON *apptid = cJSON_GetObjectItem(params, "appTid");
	cJSON *subdevtid = cJSON_GetObjectItem(params, "subDevTid");
	ASSERT_EVAL((devtid == NULL) || (ctrlkey == NULL) || (apptid == NULL) || (subdevtid == NULL),
				code, ACTION_CODE_ERROR_FORMAT);

	/*match tid*/
	dev_tid = product_info_get_devtid();
	ASSERT_EVAL((strncmp(dev_tid, devtid->valuestring, DEVTID_LENGTH_MAX) != 0),
				code, ACTION_CODE_ERROR_DEVTID);

	/*match ctrlkey*/
	ctrlkey_local = product_info_get_ctrlkey();
	ASSERT_EVAL((ctrlkey_local == NULL) || (strcmp(ctrlkey_local, ctrlkey->valuestring) != 0),
				code, ACTION_CODE_ERROR_CTRLKEY);


	uint32_t msgid_tmp = msgid->valueint;
	char *apptid_tmp = apptid->valuestring;

	//串口发送添加设备的命令
	tid_t ttid;
	ttid.len = strlen(subdevtid->valuestring) / 2;
	char *subdev_tid_value = str_to_hex(subdevtid->valuestring, strlen(subdevtid->valuestring)); //need free
	memcpy(ttid.data, subdev_tid_value, ttid.len);
	hm_48_delete_subdev_frame_send(&ttid);
	__FREE(subdev_tid_value);

	goto done;
done:
	action_resp_handle(msgid, action, params, code);
	if (code == ACTION_CODE_SUCCESS)
	{
		//删除子设备成功
	}
	return;
fail:
	action_resp_handle(msgid, action, params, code);
     //删除子设备失败
	return;
}

static void bind_dev_sync(cJSON *whole)
{
	uint32_t code = ACTION_CODE_SUCCESS;
	char *ctrlkey_local = NULL;

	ASSERT(whole == NULL);

	cJSON *request = whole;
	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *params = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_PARAMS);
	ASSERT_EVAL((msgid == NULL) || (action == NULL) || (params == NULL),
		code, ACTION_CODE_ERROR_FORMAT);

	cJSON *ctrlkey = cJSON_GetObjectItem(params, "ctrlKey");
	ASSERT_EVAL((ctrlkey == NULL), code, ACTION_CODE_ERROR_FORMAT);

	ctrlkey_local = product_info_get_ctrlkey();
	ASSERT_EVAL((ctrlkey_local == NULL) || (strcmp(ctrlkey_local, ctrlkey->valuestring) != 0),
		code, ACTION_CODE_ERROR_CTRLKEY);

	return;
fail:
	return;
}
static void bind_heartbeat_resp(cJSON *whole)
{
	cJSON *respose = whole;
	cJSON *code = cJSON_GetObjectItem(respose, "code");
	ASSERT(code == NULL);
	if (code->valueint == 200)
	{
		u_printf("HM BIND heartbeat resp success ");
		hm_clear_heartbeat_miss_times();
	}

fail:
	return;
}

static void bind_app_dev_debug(cJSON *whole)
{
	char *ctrlkey_local = NULL;
	uint32_t code = ACTION_CODE_SUCCESS;
	
	cJSON *request = whole;
	cJSON *msgid = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_MSGID);
	cJSON *action = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_ACTION);
	cJSON *params = cJSON_GetObjectItem(request, JSON_PROTOCOL_FIELD_PARAMS);
	ASSERT_EVAL((msgid == NULL) || (action == NULL) || (params == NULL),
		code, ACTION_CODE_ERROR_FORMAT);

	
	cJSON *devtid = cJSON_GetObjectItem(params, "devTid");
	ASSERT_EVAL(devtid == NULL, code, ACTION_CODE_ERROR_FORMAT);
	/*match tid*/
	char *dev_tid = product_info_get_devtid();
	ASSERT_EVAL((devtid->valuestring == NULL) || (strncmp(dev_tid, devtid->valuestring, DEVTID_LENGTH_MAX) != 0),
				code, ACTION_CODE_ERROR_DEVTID);

	cJSON *dubugFlag = cJSON_GetObjectItem(params, "enable");
	ASSERT_EVAL(dubugFlag == NULL,code, ACTION_CODE_ERROR_FORMAT);
	if (dubugFlag->valueint == true)
	{
		
	}
	else 
	{
		
	}
		
	goto done;
done:
fail:
	action_resp_handle(msgid, action, NULL, code);
	return;
}



 int bind_json_protocol_handle(char *json_str, unsigned short len)
{
	const jad_bind_item_t shm_json_protocol_actions[] = {
		{"getProdInfoResp", &bind_get_prodinfo_resp},
		{"requestVerifyResp", &bind_request_verify_resp},
		{"gatewayLoginResp", &bind_gateway_login_resp},
		{"reportDevInfoResp", &bind_report_devinfo_resp},
		{"gatewayBindResp", &bind_gateway_bind_resp},
		{"appSend", &bind_appsend},
		{"addSubDevResp", &bind_add_subdev_resp},
		{"reportSubDevInfoResp", &bind_report_subdevinfo_resp},
		{"devSendResp", &bind_dev_send_resp},
		{"devDelete", &bind_dev_delete},
		{"devSync", &bind_dev_sync},
		{"heartbeatResp" , &bind_heartbeat_resp},
		{"appDevDebug", &bind_app_dev_debug},
		{"", NULL}};

	int ret = 0;
	cJSON *whole = NULL, *action = NULL;
	char *action_str = NULL;
	size_t action_str_len = 0;
	
	/* Parse the JSON data */
	whole = cJSON_Parse(json_str);
	if (whole == NULL)
		goto fail;

	uint8 arr_len = sizeof(shm_json_protocol_actions) / sizeof(jad_bind_item_t);
			  /* Extract value of 'action' */
	//if (cJSON_HasObjectItem(whole, "action"))
	//{
		action = cJSON_GetObjectItem(whole, "action");
		if (action->type == cJSON_String)
		{
			action_str = action->valuestring;
			action_str_len = strlen(action_str);
			uint8 i = 0;
			for (i = 0; i < arr_len ;i++)
			{
				if ((strlen(shm_json_protocol_actions[i].name) == action_str_len) &&
					(strncmp(shm_json_protocol_actions[i].name, action_str, action_str_len) == 0))
				{
					/* Invoke the callback */
					u_printf("HM JSON BIND callback %s \r\n", shm_json_protocol_actions[i].name);
					shm_json_protocol_actions[i].callback(whole);
					goto done;
				}
			}
			u_printf("HM JSON BIND NOT FOUND CALLBACK  \r\n");
		}
		else
		{
			goto fail;
		}

done :
	if (whole != NULL)
		cJSON_Delete(whole);
	return ret;
fail:
	
	if (whole != NULL)
		cJSON_Delete(whole);
	return ret;
}

int  hm_cloud_json_handle(char *buf, unsigned short len)
{

	int ret = bind_json_protocol_handle(buf, len);
	return ret;
}
