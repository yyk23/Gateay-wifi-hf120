#include "ra_wifi.h"
#include "ra_timer.h"
#include <log.h>
#include <device_state.h>

void manual_start_hekr_config(size_t timeout);
void register_wifi_config_callback(ra_wifi_config_callback_t callback);

 
static void manual_wifi_connect_timeout(ra_timer_t timer, void *arg)
{
	ra_timer_delete(timer);
	if (device_state_load(STATE_WLAN_CONNECTED) == 0)
	{
		device_state_store(STATE_WLAN_CONNECT_FAILED, 1);
	}
}

 
void ra_start_wifi_config(ra_wifi_config_type_t config_type)
{
	config_status_init();
	manual_start_hekr_config(5 * 60 * 1000);
}

 
void ra_register_wifi_config_callback(ra_wifi_config_callback_t callback)
{
	register_wifi_config_callback(callback);
}

 
int ra_connect_wifi(char *ssid, char *password, char *bssid, ra_uint32_t timeout)
{
	ra_bool ret = 0;
	ra_timer_t connect_timer = 0;

	if (ssid == NULL || password == NULL)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	wifi_station_disconnect();
	//ESP8266 connect to router
	struct station_config station_conf = { { 0 } };
	if (bssid == NULL)
	{
		//need not mac address
		station_conf.bssid_set = 0;
	}
	else
	{
		station_conf.bssid_set = 1;
		memcpy(station_conf.bssid, bssid, strlen(bssid));
	}
	memcpy(station_conf.ssid, ssid, strlen(ssid) + 1);
	memcpy(station_conf.password, password, strlen(password) + 1);

	ret = wifi_station_set_config_current(&station_conf);
	if (ret != 1)
		debug_print("[error]: wifi_station_set_config ret=0");


	ra_timer_new(&connect_timer);
	ra_timer_set_callback(connect_timer, manual_wifi_connect_timeout, NULL);
	ra_timer_start(connect_timer, timeout, 0);

	return (wifi_station_connect() == true) ? RA_SUCCESS : RA_ERROR_INTERNAL;
}

 
int ra_disconnect_wifi(void)
{
	return (wifi_station_disconnect() == true) ? RA_SUCCESS : RA_ERROR_INTERNAL;
}