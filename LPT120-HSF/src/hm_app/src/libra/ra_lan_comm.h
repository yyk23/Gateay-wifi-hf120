/**
* @file         ra_lan_comm.h
* @brief        UDP�������ӿ�
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
* ra_lan_comm_recv_callback_t UDP���ݽ��ջص�����ԭ�Ͷ���
*/
typedef void(*ra_lan_comm_recv_callback_t)(ra_remote_info_t *remote, char *data, ra_uint16_t size);

/**
* UDP���ͺ���
* @param[in]	remote	Զ����Ϣ
* @param[in]	data	Ҫ���͵��ַ���
* @param[in]	size	�ַ�������
* @retval	0 �ɹ�
* @retval	-1 �ڲ�����
*/
int ra_lan_comm_send(ra_remote_info_t *remote, ra_uint8_t *data, ra_uint16_t size);

/**
* UDP server����
* @retval	0 �ɹ�
* @retval	-1 �ڲ�����
*/
int ra_lan_comm_server_start(void);

/**
* UDP server�ر�
* @retval	0 �ɹ�
* @retval	-1 �ڲ�����
*/
int ra_lan_comm_server_stop(void);

/**
* ע��UDP���ݽ��ջص�����
* @param[in]	callback	�ص�����
*/
void ra_lan_comm_register_recv_callback(ra_lan_comm_recv_callback_t callback);

#endif