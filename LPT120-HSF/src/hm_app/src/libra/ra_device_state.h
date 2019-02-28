/**
* @file         ra_device_state.h
* @brief        设备状态操作接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_DEVICE_STATE_H__
#define __RA_DEVICE_STATE_H__
#include "ra_types.h"

/**
* ra_device_state_type_t 枚举类型定义
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
* ra_device_state_changed_callback_t 函数原型定义
*/
typedef void(*ra_device_state_changed_callback_t)(ra_device_state_type_t item, ra_bool current_state);

/**
* 获取设备状态
* @param[in]	item	状态项
* @retval		ra_bool	状态项的值
*/
ra_bool ra_device_state_load(ra_device_state_type_t item);

/**
* 存储设备状态
* @param[in]	item			状态项
* @retval		current_state	状态项预设定的值
*/
void ra_device_state_store(ra_device_state_type_t item, ra_bool current_state);

/**
* 注册设备状态改变回调函数
* @param[in]	callback	ra_device_state_changed_callback_t 类型的回调函数
* @retval		无
*/
void ra_register_device_state_changed_callback(ra_device_state_changed_callback_t callback);

/**
* 设定设备状态指示灯引脚
* @param[in]	pin		LED管脚序号
* @param[in]	reverse	翻转电平
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_device_state_led_task_install(ra_uint8_t pin, ra_bool reverse);

#endif