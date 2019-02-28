#include "hsf.h"
#include "cJSON.h"
#include "hm_parameter.h"
#include "hm_system.h"
#include "hm_connect_cloud_handle.h"
#include "hm_wifi_frame.h"
#include "hm_uart_handle.h"
#include "hm_bind_json_protocol.h"
#include "hm_dev_model.h"



int hm_sub_devsend_report(hm_48_valid_data_t *valid_data)
{

    hm_48_valid_data_t *pvalid_data = valid_data;
    cJSON *request = NULL;
    char *request_str = NULL;
    u_printf("HM TCP SEND hm_sub_devsend_report\r\n");
    request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_DEVSEND);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);

    char *tid = product_info_get_devtid();
    char *subtid = hex_to_str(&(pvalid_data->tid.data[0]), pvalid_data->tid.len, false); //need free
    cJSON_AddStringToObject(params, JSON_PROTOCOL_FIELD_DEVTID, tid ? tid : "");
    cJSON_AddStringToObject(params, JSON_PROTOCOL_FIELD_APPTID, "");

    cJSON_AddStringToObject(params, JSON_PROTOCOL_FIELD_SUBDEVTID, subtid ? subtid : "");

    cJSON *data = cJSON_CreateObject();
    cJSON_AddItemToObject(params, JSON_PROTOCOL_FIELD_DATA, data);
    //如果是水龙头设备
    if (strcmp(pvalid_data->dev_type, WT_DEV_TYPE_NAME) == 0)
    {
        u_printf("HM　hm_sub_devsend_report dev type is ShuiLongTou \r\n");
        cJSON_AddNumberToObject(data, JSON_PROTOCOL_FIELD_CMDID, pvalid_data->cmd_id);
        //检测是否有此命令ID
        cmd_info_t * tcmd_info = hm_check_cmd_id(pvalid_data->dev_type, pvalid_data->cmd_id);
        if(tcmd_info == NULL)  goto done;
        uint8 i = 0;
        for (i = 0; i < (tcmd_info->attr_num) ; i++)
        {
            char *tattr_name = hm_get_attr_name(pvalid_data->dev_type, tcmd_info->attr_list[i]);
            u_printf("HM　hm_sub_devsend_report attr_name is %s \r\n", tattr_name);
            if(tattr_name == NULL) break;
            char *tattr_datatype = hm_get_attr_datatype(pvalid_data->dev_type, tcmd_info->attr_list[i]);
            u_printf("HM　hm_sub_devsend_report attr_datatype is %s \r\n", tattr_datatype);
            if(tattr_datatype == NULL) break;
            uint32 tattr_value = 0;
            INT32(tattr_value, pvalid_data->attrs_data[i*4], pvalid_data->attrs_data[i*4+1], pvalid_data->attrs_data[i*4+2], pvalid_data->attrs_data[i*4+3]);
            u_printf("HM　hm_sub_devsend_report attr_value is %d \r\n", tattr_value);
            cJSON_AddNumberToObject(data, tattr_name, tattr_value);
        }
       
    }

done :
    request_str = cJSON_PrintUnformatted(request);
    cJSON_Delete(request);
    request = NULL;
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));

   
    if (subtid != NULL)
       { __FREE(subtid);}
    return 0;
fail:
    if (subtid != NULL)
    {
        __FREE(subtid);
    }
    return -1 ;
}

int hm_add_sub_dev_report(tid_t *stid , mid_t  *smid , uint8 online_status)
{
    cJSON *request = NULL;
    char *request_str = NULL;
    u_printf("HM TCP SEND hm_add_sub_dev_report\r\n");
    request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_ADDSUBDEV);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);

    char *tid = product_info_get_devtid();
    char *subtid = hex_to_str(stid->data , stid->len, false); //need free
    char submid[MAX_MID_BYTES_LENTH + 1] ={0};
    memcpy(submid, smid->data, smid->len);
    submid[smid->len] = '\0';
    cJSON_AddStringToObject(params, JSON_PROTOCOL_FIELD_DEVTID, tid ? tid : "");
    cJSON_AddStringToObject(params, JSON_PROTOCOL_FIELD_SUBDEVTID, subtid ? subtid : "");
    cJSON_AddStringToObject(params, "subMid", submid);
    cJSON_AddStringToObject(params, "subConnectType", "ZigBee_HA");
    cJSON_AddBoolToObject(params, "online", online_status);
    cJSON_AddStringToObject(params, "binVer", "1.0.0");
    cJSON_AddStringToObject(params, "subDevName", "WT");
    cJSON_AddStringToObject(params, "subDevModel", "WT-1");
    cJSON_AddStringToObject(params, "manufacturerName", "GOLD");
    cJSON_AddStringToObject(params, "description", "");

    request_str = cJSON_PrintUnformatted(request);
    cJSON_Delete(request);
    request = NULL;
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));

fail:
    if (subtid != NULL)
        __FREE(subtid);
  
    return 0;
}

int hm_sub_dev_status_report(tid_t *stid, mid_t *smid, uint8 online_status)
{
    cJSON *request = NULL;
    char *request_str = NULL;
    u_printf("HM TCP SEND hm_sub_dev_status_report\r\n");
    request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_REPORTSUBDEVINFO);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);

    char *tid = product_info_get_devtid();
    char *subtid = hex_to_str(stid->data, stid->len, false); //need free
    char submid[MAX_MID_BYTES_LENTH + 1] = {0};
    memcpy(submid, smid->data, smid->len);
    submid[smid->len] = '\0';
    cJSON_AddStringToObject(params, JSON_PROTOCOL_FIELD_DEVTID, tid ? tid : "");
    cJSON_AddStringToObject(params, JSON_PROTOCOL_FIELD_SUBDEVTID, subtid ? subtid : "");
    cJSON_AddStringToObject(params, "subMid", submid);
    cJSON_AddStringToObject(params, "subConnectType", "ZigBee_HA");
    cJSON_AddBoolToObject(params, "online", online_status);
    cJSON_AddStringToObject(params, "binVer", "1.0.0");
    cJSON_AddStringToObject(params, "subDevName", "WT");
    cJSON_AddStringToObject(params, "subDevModel", "WT-1");
    cJSON_AddStringToObject(params, "manufacturerName", "GOLD");
    cJSON_AddStringToObject(params, "description", "");

    request_str = cJSON_PrintUnformatted(request);
    cJSON_Delete(request);
    request = NULL;
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));

fail:
    if (subtid != NULL)
        __FREE(subtid);
    return 0;

}
