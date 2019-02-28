/**
* @file         ra_uart.h
* @brief        串口操作接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_UART_H__
#define __RA_UART_H__
#include "ra_types.h"

/**
* ra_uart_id_t 枚举类型定义
*/
typedef enum
{
	RA_UART0 = 0,
	RA_UART1,
} ra_uart_id_t;

typedef enum
{
	RA_UART_RATE_UNCHANGED = 0,
	RA_UART_RATE_4800 = 4800,
	RA_UART_RATE_9600 = 9600,
	RA_UART_RATE_19200 = 19200,
	RA_UART_RATE_38400 = 38400,
	RA_UART_RATE_57600 = 57600,
	RA_UART_RATE_74880 = 74880,
	RA_UART_RATE_115200 = 115200,
	RA_UART_RATE_230400 = 230400,
	RA_UART_RATE_460800 = 460800,
	RA_UART_RATE_921600 = 921600
} ra_uart_rate_t;

typedef enum
{
	RA_UART_DATA_BITS_5 = 0,
	RA_UART_DATA_BITS_6 = 1,
	RA_UART_DATA_BITS_7 = 2,
	RA_UART_DATA_BITS_8 = 3
} ra_uart_data_bits_t;

typedef enum
{
	RA_UART_PARITY_NONE = 0x00,
	RA_UART_PARITY_EVEN = 0x01,
	RA_UART_PARITY_ODD = 0x03,
} ra_uart_parity_t;

typedef enum
{
	RA_UART_STOP_BITS_1 = 1,
	RA_UART_STOP_BITS_15 = 2,
	RA_UART_STOP_BITS_2 = 3
} ra_uart_stop_bits_t;

/**
* ra_recv_uart_data_callback_t UART数据接收回调函数原型定义
*/
typedef void(*ra_recv_uart_data_callback_t)(char *buf, ra_size_t len);

/**
* 串口接收中断使能
* @param[in]	uart_id	串口id
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_recv_enable(ra_uart_id_t uart_id);

/**
* 串口接收中断禁用
* @param[in]	uart_id	串口id
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_recv_disable(ra_uart_id_t uart_id);

/**
* 串口发送函数
* @param[in]	uart_id	串口id
* @param[in]	buf		要发送的字符串
* @param[in]	len		字符串长度
* @retval	0 成功
* @retval	-1 内部错误
* @retval	-3 参数错误
*/
int ra_uart_send_data(ra_uart_id_t uart_id, char *buf, ra_size_t len);

/**
* 设置串口数据发送队列间隔
* @param[in]	uart_id	串口ID
* @param[in]	interval	间隔（毫秒）
* @retval	0 成功
* @retval	-3 参数错误
* @note	interval表示帧间间隔时间, 默认100。最小值为10
*/
int ra_uart_set_sending_queue_interval(ra_uart_id_t uart_id, ra_uint32_t interval);

/**
* 设置串口波特率
* @param[in]	uart_id	串口ID
* @param[in]	rate	串口波特率
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_set_rate(ra_uart_id_t uart_id, ra_uart_rate_t rate);

/**
* 设置串口数据位
* @param[in]	uart_id	串口ID
* @param[in]	data_bits	串口数据位
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_set_data_bits(ra_uart_id_t uart_id, ra_uart_data_bits_t data_bits);

/**
* 设置串口停止位
* @param[in]	uart_id	串口ID
* @param[in]	stop_bits	串口停止位
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_set_stop_bits(ra_uart_id_t uart_id, ra_uart_stop_bits_t stop_bits);

/**
* 设置串口校验模式
* @param[in]	uart_id	串口ID
* @param[in]	parity	串口校验模式
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_set_parity(ra_uart_id_t uart_id, ra_uart_parity_t parity);

/**
* 设置串口接收超时
* @param[in]	uart_id	串口ID
* @param[in]	timeout	串口接收超时（毫秒）
* @retval	0 成功
* @retval	-3 参数错误
* @note	timeout的值默认100。最小值为10
*/
int ra_uart_set_recv_timeout(ra_uart_id_t uart_id, ra_uint32_t timeout);

/**
* 设置串口接收回调触发长度
* @param[in]	uart_id	串口ID
* @param[in]	len	长度(最大值为256)
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_set_recv_buffer_size_limit(ra_uart_id_t uart_id, ra_uint16_t len);

/**
* 注册串口数据接收回调函数
* @param[in]	uart_id	串口ID
* @param[in]	callback	ra_recv_uart_data_callback_t 类型的回调函数
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_register_recv_callback(ra_uart_id_t uart_id, ra_recv_uart_data_callback_t callback);

/**
* 设置打印终端串口
* @param[in]	uart_id	串口ID
* @retval	0 成功
* @retval	-3 参数错误
*/
int ra_uart_set_terminal(ra_uart_id_t uart_id);

#endif