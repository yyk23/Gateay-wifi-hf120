#include "hsf.h"
#include <hm_connect_cloud.h>
#include <hm_bind_json_protocol.h>
#include "ra_utils.h"
#include "ra_cloud.h"

 
void ra_register_cloud_recv_callback(ra_recv_cloud_msg_callback_t callback)
{
	register_cloud_reicve_callback(callback);
}

 
void ra_connect_to_cloud(void)
{
	connect_server();
}

 
void ra_disconnect_from_cloud(void)
{
	disconnect_from_server();
}

 
void ra_send_data_to_cloud(char *buf, ra_int16_t len)
{
	if (device_state_load(STATE_LOGGED_IN_SERVER) == 1)
	{
		send_data_to_cloud(buf, len);
	}
	else
	{
		printf("cloud disconnected\n");
	}
}

 
void ra_enable_cloud_data_parse(void)
{
	enable_cloud_data_parse();
}

 
void ra_register_appsend_parse_callback(ra_appsend_parse_callback_t callback)
{
	ma_register_appsend_parse_callback((ma_appsend_parse_callback_t)callback);
}

 
void ra_register_appsend_finished_callback(ra_callback_t callback)
{
	ma_register_appsend_finished_callback((callbcak_t)callback);
}

 
void ra_register_gettimerlistresp_parse_callback(ra_gettimerlistresp_parse_callback_t callback)
{
	ma_register_timer_order_callback((ma_timer_order_callback_t)callback);
}