#include "hsf.h"
#include "ra_upgrade.h"
#include <hm_system.h>
#include <log.h>
#include "ra_version.h"
#include "cJSON.h"
#include "upgrade.h"

#define UPDATE_CHECK_SERVER_DOMAIN	"poseidon.hekr.me"
#define UPGRADE_TIMEOUT			10000	//ms
#define UPGRADE_HTTPHEAD_SZIE	512

#define sec				1
#define hour			(3600*sec)
#define day				(24*hour)

#if defined(__DEBUG__)
#define UPDATE_PERIOD	(hour*1)
#else
#define UPDATE_PERIOD	(hour*1)
#endif
bool start_update(char *URL, char *MD5, char firmware_type);
void stop_update(void);
uint32_t get_update_user_bin_addr(void);



#define HTTP_HEAD "Connection: keep-alive\r\n\
Cache-Control: no-cache\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36 \r\n\
Accept: */*\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: zh-CN,zh;q=0.8,en;q=0.6\r\n\r\n"


ETSTimer g_update_timer;

typedef struct
{
	struct _esp_tcp tcp;
	struct espconn connect;
	struct upgrade_server_info server;
	struct {
		ip_addr_t ip;
		char *str;
		uint16_t port;
	}host;
	char *bin_dir;
}update_info_t;

/*sava all about upgrade info*/
update_info_t *gp_update_info = NULL;

typedef struct {
	ra_dev_upgrade_success_callback_t success_callback;
	ra_dev_upgrade_failed_callback_t failed_callback;
}update_callback_t;
static update_callback_t update_callback;


 
update_info_t * update_info_creat(void)
{
	/*init struct*/
	update_info_t * update_info = (update_info_t *)zalloc(sizeof(update_info_t));
	if (update_info == NULL)
	{
		ERR_MSG("malloc error");
		goto fail;
	}
	update_info->connect.proto.tcp = &update_info->tcp;
	update_info->connect.type = ESPCONN_TCP;
	update_info->connect.state = ESPCONN_NONE;
	goto done;
done:
	return update_info;
fail:
	return NULL;
}

  inline
void  update_info_free(void)
{
	printf("[OTA]	update_info_free\n");
	if (gp_update_info == NULL)
		return;
	__FREE(gp_update_info->bin_dir);
	__FREE(gp_update_info->host.str);
	__FREE(gp_update_info->server.url);
	__FREE(gp_update_info);
}


 
void  stop_update(void)
{
	printf("[OTA]	stop_update \n");
	device_state_store(STATE_UPDATE_RUNNING, 0);

	update_info_free();

	device_state_store(STATE_UPDATE_LOCK, 0);
	device_state_store(STATE_COMPULSORY_UPDATE, 0);
}


 
void updata_result_send_to_cloud(char upgradeProgress,char state)
{
#define msg_format "{"\
		"\"msgId\":%u,"\
			"\"action\":\"devSend\","\
			"\"params\":{"\
			"\"devTid\":\"%s\","\
				"\"appTid\":[],"\
				"\"data\":{"\
				"\"upgradeProgress\":%u,"\
				"\"upgradeState\":%u"\
			"}"\
		"}"\
	"}"

		RODATA_STR(msg, msg_format);
		GET_TID(devtid);
		char data[sizeof(msg_format) + DEVTID_LENGTH_MAX + sizeof(UINT8_MAX_STR) + 2 * sizeof(UINT32_MAX_STR)];
		size_t sprint_size = sprintf(data, msg_format, cloud_creat_msgid(), devtid, upgradeProgress, state);
		send_data_to_cloud(data, sprint_size);
		if (system_upgrade_flag_check() == UPGRADE_FLAG_FINISH)
		{
			TIMER_REGISTER(&g_update_timer, system_upgrade_reboot, NULL, 2000, 0);
		}
}

  static
void check_update_result(void *arg)
{
	uint32 roll = 0;

	roll =	system_get_time()%5000;
	printf("[OTA]	check_update_result\n");
	struct upgrade_server_info *server = arg;

	if (system_upgrade_flag_check() == UPGRADE_FLAG_FINISH)
	{
		if (device_state_load(STATE_MCU_UPDATE))
		{
			device_state_store(STATE_MCU_UPDATE,0);
			system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
		}
		else
		{
			if (update_callback.success_callback != NULL)
			{
				update_callback.success_callback();
			}
			else
			{
				register_login_success_callback_for_ota(server->progress,server->state,updata_result_send_to_cloud);
			}
		}
	}
	else
	{
		if (device_state_load(STATE_MCU_UPDATE))
		{
			device_state_store(STATE_MCU_UPDATE,0);
			system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
		}
		else
		{
			if (update_callback.failed_callback != NULL)
			{
				update_callback.failed_callback(-1);
			}
			else
			{
				register_login_success_callback_for_ota(server->progress,server->state,updata_result_send_to_cloud);
			}
		}
	}
	printf("[OTA]	reconnent cloud %4u\n",roll);
	reconnect_cloud_delay(roll);
	stop_update();
}



  static
size_t creat_get_bin_request(char **purl, char *bin_dir, char *host_str)
{
#define URL "GET %s HTTP/1.0\r\nHost: %s\r\n"HTTP_HEAD""
	RODATA_STR(url, URL);
	debug_print("update server connect succeed,will start get bin\r\n");
	debug_print_str(bin_dir);
	debug_print_str(host_str);
	size_t url_buf_size = sizeof(url) + strlen(bin_dir) + strlen(host_str);
	*purl = malloc(url_buf_size);
	ASSERT(*purl == NULL);

	size_t sprintf_size = sprintf(*purl, url, bin_dir, host_str);
	ASSERT(sprintf_size >= url_buf_size);
	goto done;
done:
	return sprintf_size;
fail:
	free(*purl);
	stop_update();
	return 0;
#undef URL
}


  void
update_sever_ip_found(const char *name, ip_addr_t *ipaddr, void *arg)
{
	if (ipaddr == NULL || ipaddr->addr == 0)
	{
		ERR_MSG("LocalUpgradeSeverip not Found will reconn_update_server");
		goto fail;
	}

	//dns got ip
	debug_print("DNS OK,%s %d.%d.%d.%d",
		name,
		*((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr + 1),
		*((uint8 *)&ipaddr->addr + 2), *((uint8 *)&ipaddr->addr + 3));

	os_timer_disarm(&g_update_timer);

	/*init struct sever*/
	gp_update_info->server.pespconn = (struct espconn *)arg;
	gp_update_info->server.port = (uint16)gp_update_info->host.port;
	gp_update_info->server.check_cb = check_update_result;
	gp_update_info->server.check_times = UPGRADE_TIMEOUT;
	memcpy(gp_update_info->server.ip, &(ipaddr->addr), 4);
	memcpy(gp_update_info->server.pespconn->proto.tcp->remote_ip, &ipaddr->addr, 4);
	gp_update_info->server.pespconn->type = ESPCONN_TCP;
	gp_update_info->server.pespconn->state = ESPCONN_NONE;
	gp_update_info->server.pespconn->reverse = &gp_update_info->server;
	gp_update_info->server.pespconn->proto.tcp->remote_port = (uint16)gp_update_info->host.port;
	gp_update_info->server.pespconn->proto.tcp->local_port = espconn_port();
	ASSERT(creat_get_bin_request((char **)&gp_update_info->server.url,
		gp_update_info->bin_dir,
		gp_update_info->host.str) == 0);


	system_upgrade_flag_set(UPGRADE_FLAG_IDLE);
	ASSERT(system_upgrade_start(&gp_update_info->server) == false);
	disconnect_from_server();
	unregister_cloud_hearbeat();
	goto done;
done:
	return;
fail:
	stop_update();
	return;
}


 
bool start_update(char *URL, char *MD5, char firmware_type)
{
	char *http_head_host = NULL;

	char *host_port = NULL;
	char *host_domain = NULL;
	char *HTTP_GET_URL = NULL;
	uint16_t port = 80;

	device_state_store(STATE_UPDATE_RUNNING, 1);
	debug_print("start_update\n");

	/*check firmware type*/
	ASSERT(((firmware_type != get_update_firmware_type()) && (firmware_type != 'M')) ||(MD5 == NULL) ||(URL == NULL));
	if (get_str_from_str(URL, "http://", "/", &http_head_host, &HTTP_GET_URL) == true)
	{
		char *remain = NULL;
		/*get host domain*/
		ASSERT(http_head_host == NULL);
		if (get_str_from_str(http_head_host, "", ":", &host_domain, &remain) == true)
		{
			/*get host port*/
			if (remain != NULL)
			{
				get_str_from_str(remain, ":", "", &host_port, NULL);
				free(remain);
				port = host_port != NULL ? atoi(host_port) : port;
			}
		}
		else
		{
			host_domain = str_clone(http_head_host, strlen(http_head_host));
			ASSERT(host_domain == NULL);
		}
	}
	else
	{
		ERR_MSG("start_update:url error");
		goto fail;
	}

	/*get server port*/
	if (gp_update_info == NULL)
	{
		/*init */
		gp_update_info = update_info_creat();
		ASSERT(gp_update_info == NULL);
	}
	gp_update_info->host.str = http_head_host;
	gp_update_info->bin_dir = HTTP_GET_URL;
	/*if domain is ip*/
	get_ip_from_str((uint32_t *)&gp_update_info->host.ip.addr, host_domain);

	gp_update_info->host.port = port;
	lib_strtohex(gp_update_info->server.MD5, MD5, 32);
	if (gp_update_info->host.ip.addr == 0)
	{
		espconn_gethostbyname(
			&gp_update_info->connect, host_domain,
			&gp_update_info->host.ip, update_sever_ip_found);
		goto done;
	}
	else
	{
		update_sever_ip_found(host_domain, &(gp_update_info->host.ip), &(gp_update_info->connect));
	}
	goto done;

done:
	free(host_domain);
	free(host_port);
	return true;
fail:
	free(http_head_host);
	free(host_domain);
	free(host_port);
	free(HTTP_GET_URL);
	stop_update();
	return false;
}


 
uint32_t get_update_user_bin_addr(void)
{
	return system_upgrade_userbin_check() == UPGRADE_FW_BIN1 ? USER_BIN2_ADDR : USER_BIN1_ADDR;
}

 
int ra_start_dev_upgrade(char *dev_tid, char *bin_url, char *md5, char *bin_type)
{
	GET_TID(local_tid);
	/*match tid*/
	if (strncmp(local_tid, dev_tid, strlen(local_tid)) != 0)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	if (system_upgrade_flag_check() == UPGRADE_FLAG_START)
	{
		return RA_ERROR_INTERNAL;
	}
	/*check firmware type*/
	if (strlen(bin_type) != 1 || *bin_type != get_update_firmware_type())
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	device_state_store(STATE_COMPULSORY_UPDATE, 1);
	return (start_update(bin_url, md5, *bin_type) == ra_true) ? RA_SUCCESS : RA_ERROR_INVALID_ARGUMENTS;

}

 
void ra_system_upgrade_reboot(void)
{
	system_upgrade_reboot();
}

 
void ra_register_dev_upgrade_success_callback(ra_dev_upgrade_success_callback_t callback)
{
	update_callback.success_callback = callback;
}

 
void ra_register_dev_upgrade_failed_callback(ra_dev_upgrade_failed_callback_t callback)
{
	update_callback.failed_callback = callback;
}