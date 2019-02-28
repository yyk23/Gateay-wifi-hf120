/**
* @file         ra_device_state.h
* @brief        �豸״̬�����ӿ�
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_DEVICE_STATE_H__
#define __RA_DEVICE_STATE_H__
#include "ra_types.h"

/**
* ra_device_state_type_t ö�����Ͷ���
*/
typedef enum
{
	RA_DEVICE_STATE_WLAN_CONNECTED = 0,
	RA_DEVICE_STATE_WLAN_CONNECT_FAILED,
	RA_DEVICE_STATE_CLOUD_CONNECTED,
	RA_DEVICE_STATE_HEKR_CONFIG_RUNNING,
	RA_DEVICE_STATE_AIRKISS_CONFIG_RUNNING,
} ra_device_state_type_t;


/**
* ra_device_state_changed_callback_t ����ԭ�Ͷ���
*/
typedef void(*ra_device_state_changed_callback_t)(ra_device_state_type_t item, ra_bool current_state);

/**
* ��ȡ�豸״̬
* @param[in]	item	״̬��
* @retval		ra_bool	״̬���ֵ
*/
ra_bool ra_device_state_load(ra_device_state_type_t item);

/**
* �洢�豸״̬
* @param[in]	item			״̬��
* @retval		current_state	״̬��Ԥ�趨��ֵ
*/
void ra_device_state_store(ra_device_state_type_t item, ra_bool current_state);

/**
* ע���豸״̬�ı�ص�����
* @param[in]	callback	ra_device_state_changed_callback_t ���͵Ļص�����
* @retval		��
*/
void ra_register_device_state_changed_callback(ra_device_state_changed_callback_t callback);

/**
* �趨�豸״ָ̬ʾ������
* @param[in]	pin		LED�ܽ����
* @param[in]	reverse	��ת��ƽ
* @retval	0 �ɹ�
* @retval	-3 ��������
*/
int ra_device_state_led_task_install(ra_uint8_t pin, ra_bool reverse);

#endif