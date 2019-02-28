/**
* @file         ra_cloud.h
* @brief        服务器操作接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __RA_CLOUD_H__
#define __RA_CLOUD_H__

#include "ra_types.h"
#include <cJSON.h>

/* code 取值枚举*/
#define CODE_SUCCESS				200		//成功
#define CODE_ERROR_PARSE_ERROR		2000102	//JSON解析失败	
#define CODE_ERROR_ACTION_UNKNOWN	2000103	//action无效
#define CODE_ERROR_PARAMS			2000106	//参数不存在
#define CODE_ERROR_DEVTID			2000107	//devTid错误
#define CODE_ERROR_CTRLKEY			2000109	//ctrlKey错误
#define CODE_ERROR_EXECUTION 		2000114	//执行失败
#define CODE_ERROR_BIN_TYPE			2000115	//OTA升级时，固件类型(A/B)错误
#define CODE_ERROR_UPDATE_STARTED 	2000116	//OTA升级时，设备正在升级
#define CODE_ERROR_INTERNAL_ERROR 	2000118	//内部错误
#define CODE_ERROR_VALUE_INVALID	2000120	//参数值错误
#define CODE_ERROR_CMDID_INVALID	2000121	//cmdId无效
/*end*/

/**
* 云端数据接收回调函数原型定义
*/
typedef void(*ra_recv_cloud_msg_callback_t)(char *buf, ra_uint16_t len);

/**
* appSend回调函数原型定义
* @param[in]	params	指向appSend->data，用户只需处理data
* @retval		200		成功，appSendResp中code = 200
* @retval		2000114	执行失败，appSendResp中code = 2000114
* @retval		2000106 参数不存在，appSendResp中code = 2000106
* @retval		2000120	参数值错误，appSendResp中code = 2000120
* @retval		2000121	cmdId无效，appSendResp中code = 2000121
*/
typedef ra_uint32_t(*ra_appsend_parse_callback_t)(cJSON *params);

/**
* getTimerListResp回调函数原型定义
* @param[in]	params	指向getTimerListResp->taskList->task，用户只需处理task
* @retval		200		成功，timerReport中code = 200
* @retval		2000114	执行失败，timerReport中code = 2000114
* @retval		2000106 参数不存在，timerReport中code = 2000106
* @retval		2000120	参数值错误，timerReport中code = 2000120
* @retval		2000121	cmdId无效，timerReport中code = 2000121	
*/
typedef ra_uint32_t(*ra_gettimerlistresp_parse_callback_t)(cJSON *params);

/**
* 注册云端数据接收的回调函数
* @param[in]	callback	回调函数
* @retval		无
* @note			用户可以在回调函数中自行处理云端数据。如果用户希望内部处理云端数据(用户只需关心用户数据)，请参考ra_enable_cloud_data_parse()。
*/
void ra_register_cloud_recv_callback(ra_recv_cloud_msg_callback_t callback);

/**
* 连接服务器
* @retval		无
*/
void ra_connect_to_cloud(void);

/**
* 断开服务器
* @retval		无
*/
void ra_disconnect_from_cloud(void);

/**
* 发送数据到云端
* @param[in]	buf		要发送的字符串
* @param[in]	len		字符串长度
* @retval	0 无
*/
void ra_send_data_to_cloud(char *buf, ra_int16_t len);

/**
* 使能内部处理云端数据
* @retval		无
* @note			使能后，内部会处理云端数据，用户数据被解析成cJSON结构体，并通过回调函数传递给用户，请参考ra_register_appsend_parse_callback()和ra_register_gettimerlistresp_parse_callback。
* @note			调用该函数后，不能调用ra_register_cloud_recv_callback()。
*/
void ra_enable_cloud_data_parse(void);

/**
* 注册appSend任务执行的回调函数
* @param[in]	callback	appSend任务执行的回调函数
* @retval		无
*/
void ra_register_appsend_parse_callback(ra_appsend_parse_callback_t callback);

/**
* 注册appSend任务执行完成后的回调函数
* @param[in]	callback	appSend任务执行完成后的回调函数
* @retval		无
*/
void ra_register_appsend_finished_callback(ra_callback_t callback);

/**
* 注册getTimerListResp定时任务执行的回调函数
* @param[in]	callback	定时任务执行的回调函数
* @retval		无
* @note			接收到定时任务后，内部会维护任务时间，待执行时间到达时触发回调函数。
*/
void ra_register_gettimerlistresp_parse_callback(ra_gettimerlistresp_parse_callback_t callback);

#endif