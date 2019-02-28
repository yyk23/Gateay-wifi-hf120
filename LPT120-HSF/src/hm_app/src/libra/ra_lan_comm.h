/**
* @file         ra_lan_comm.h
* @brief        UDP局域网接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/
#ifndef __RA_LAN_COMM_H__
#define __RA_LAN_COMM_H__
#include "ra_types.h"

typedef struct {
	ra_int32_t remote_port;
	ra_uint8_t remote_ip[4];
}ra_remote_info_t;

typedef struct {
	ra_int32_t remote_port;
	ra_uint8_t remote_ip[4];
}ra_juremote_info_t;

/**
* ra_lan_comm_recv_callback_t UDP数据接收回调函数原型定义
*/
typedef void(*ra_lan_comm_recv_callback_t)(ra_remote_info_t *remote, char *data, ra_uint16_t size);

/**
* UDP发送函数
* @param[in]	remote	远端信息
* @param[in]	data	要发送的字符串
* @param[in]	size	字符串长度
* @retval	0 成功
* @retval	-1 内部错误
*/
int ra_lan_comm_send(ra_remote_info_t *remote, ra_uint8_t *data, ra_uint16_t size);

/**
* UDP server启动
* @retval	0 成功
* @retval	-1 内部错误
*/
int ra_lan_comm_server_start(void);

/**
* UDP server关闭
* @retval	0 成功
* @retval	-1 内部错误
*/
int ra_lan_comm_server_stop(void);

/**
* 注册UDP数据接收回调函数
* @param[in]	callback	回调函数
*/
void ra_lan_comm_register_recv_callback(ra_lan_comm_recv_callback_t callback);

#endif