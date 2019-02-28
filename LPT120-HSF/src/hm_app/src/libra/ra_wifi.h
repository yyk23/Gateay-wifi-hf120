/**
* @file         ra_wifi.h
* @brief        ����������·�ɽӿ�
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_WIFI_H__
#define __RA_WIFI_H__
#include "ra_types.h"

/**
* �����ķ�ʽ
*/
typedef enum
{
	RA_WIFI_CONFIG_TYPE_HEKR = 0,
	RA_WIFI_CONFIG_TYPE_AIRKISS
} ra_wifi_config_type_t;


/**
* ra_wifi_config_callback_t ����ԭ�Ͷ���
*/
typedef void(*ra_wifi_config_callback_t)(char *ssid, char *password);


/**
* ��wifi config
* @param[in]	config_type
* @retval		��
*/
void ra_start_wifi_config(ra_wifi_config_type_t config_type);


/**
* ע��wifi config�����ص�����
* @param[in]	callback
* @retval		��
* @note		�ڻص������У�����ʧ����ssid����NULL
*/
void ra_register_wifi_config_callback(ra_wifi_config_callback_t callback);

/**
* ����WIFI�ӿ�
* @param[in]  ssid  WIFI�ȵ�����
* @param[in]  password  WIFI�ȵ�����
* @param[in]  bssid BSSID(��ΪNULL)
* @param[in]  timeout ���ӳ�ʱʱ�䣨ms��
* @retval	0        ��ʼ���ӳɹ�
* @retval	-1       �ڲ�����
* @retval	-3       ��������
*/
int ra_connect_wifi(char *ssid, char *password, char *bssid, ra_uint32_t timeout);

/**
* �Ͽ�wifi����
* @retval	0        ��ʼ���ӳɹ�
* @retval	-1       �ڲ�����
*/
int ra_disconnect_wifi(void);

#endif