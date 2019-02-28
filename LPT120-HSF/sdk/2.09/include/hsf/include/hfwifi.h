
/* hfwifi.h
 *
 * Copyright (C) 2014-2015 ShangHai High-flying Electronics Technology Co.,Ltd.
 *
 * This file is part of HSF.
 * 
 * Modify:
 * 2015-10-10 : Create by Jim
 */

 #ifndef __HF_WIFI_H_H__
 #define __HF_WIFI_H_H__

#define AUTH_TYPE_OPEN				0
#define AUTH_TYPE_SHARED			1
#define AUTH_TYPE_WPAPSK			2
#define AUTH_TYPE_WPA2PSK			3
#define AUTH_TYPE_WPAPSKWPA2PSK		4
#define AUTH_TYPE_NUM				5
 
#define ENC_TYPE_NONE					0
#define ENC_TYPE_WEP					1
#define ENC_TYPE_TKIP					3
#define ENC_TYPE_AES					4
#define ENC_TYPE_TKIPAES				5
#define ENC_TYPE_NUM					6

typedef enum _WIFI_PS_MODE
{
	WIFI_PS_AUTO=0,
	WIFI_PS_ON=1,	
	WIFI_PS_OFF
}WIFI_PS_MODE_E;

typedef struct _WIFI_SCAN_RESULT_ITEM
 {
	uint8_t auth;
	uint8_t encry;
	uint8_t channel;
	uint8_t rssi;
	char    ssid[32+1];
	uint8_t mac[6];
	int       rssi_dbm;
	int       sco;
 }WIFI_SCAN_RESULT_ITEM,*PWIFI_SCAN_RESULT_ITEM;

typedef int (*hfwifi_scan_callback_t)( PWIFI_SCAN_RESULT_ITEM );


int HSF_API hfwifi_scan(hfwifi_scan_callback_t p_callback);

int HSF_API hfwifi_scan_ex(hfwifi_scan_callback_t p_callback,uint8_t min_ch,uint8_t max_ch,uint16_t delay_ms,uint8_t force,struct process *p_process);

int HSF_API hfwifi_enable_ap_idle_auto_reset(int max_idle_time);

bool HSF_API hfwifi_get_existing_ssid_rssi(uint8_t *ssid, int *crssi);

bool HSF_API hfwifi_set_scan_ssid(uint8_t *ssid);

void HSF_IAPI hfwifi_init(void);

int HSF_API hfwifi_transform_rssi(int rssi_dbm);

int HSF_API hfwifi_sta_get_current_rssi(int *dBm);

int HSF_IAPI hfwifi_write_sta_mac_address(uint8_t *mac);

int HSF_IAPI hfwifi_read_sta_mac_address(uint8_t *mac);

void HSF_IAPI hfwifi_scan_result_raw_format_str(void *result,char *str);

int HSF_API hfwifi_powersave(int enable);

int HSF_API hfwifi_sleep(void);

void HSF_IAPI hfwifi_scan_result_format_str(PWIFI_SCAN_RESULT_ITEM item,char *str);

int HSF_API hfwifi_softap_connected_sta_cnt(void);

int HSF_API hfwifi_is_scanning(void);

void hfwifi_scan_result_format_web_str(WIFI_SCAN_RESULT_ITEM *item,char *dst);


#endif


