/**
* @file         hm_parameter.h
* @brief        参数存取操作接口
* @par Copyright (c):
*       Hangzhou District Nine Technology Co., Ltd.
*/

#ifndef __HM_PARAMETER_H__
#define __HM_PARAMETER_H__
#include "ra_types.h"
#include "c_types.h"

#define SPI_FLASH_SEC_SIZE 4096U

#define ESP_PARAM_START_SEC				0x01
#define ESP_FEEDBACKPARAM_START_SEC		0x04



#define KEY_VALUE_BUF_LEN_DEFAULT 2*1024

#define KEY_LENGTH_MAX 32
#define VALUE_LENGTH_MAX 64

#define BUF_LEN_LEN 4
#define KEY_VALUE_SIGN '#'
#define KEY_VALUE_SIGN_LEN 1
#define SPACE_MARK ' '
#define SPACE_MARK_LEN 1
#define VALUE_TYPE_MARK_LEN 1

typedef enum
{
	SPI_FLASH_RESULT_OK,
	SPI_FLASH_RESULT_ERR,
	SPI_FLASH_RESULT_TIMEOUT
} SpiFlashOpResult;

typedef enum {
	VALUE_TYPE_INT = 0,
	VALUE_TYPE_STR
}value_type_t;

typedef struct {
	struct
	{
		char *str;
		ra_size_t len;
	}key;
	value_type_t value_type;
	union
	{
		ra_int32_t value_int;
		struct
		{
			char * str;
			ra_size_t len;
		}value_str;

	}u;
}key_value_t;

int spi_flash_synchronous_with_protect(uint32 src_addr);
/** 设置整数类型参数
* @param[in]  key   参数名
* @param[in]  value 参数值
* @retval	0       设置成功
* @retval	-2      内存溢出
* @retval	-3      参数错误
*/
int ra_set_parameter_integer(const char *key, ra_int32_t value);

/** 获取整数类型参数
* @param[in]  key   参数名
* @param[out]  value   参数值
* @retval	0       获取成功
* @retval	-1      内部错误
*/
int ra_get_parameter_integer(const char *key, ra_int32_t *value);

/** 设置字符串类型参数
* @param[in]  key   参数名
* @param[in]  value 参数值
* @retval	0       设置成功
* @retval	-2      内存溢出
* @retval	-3      参数错误
*/
int ra_set_parameter_string(const char *key, const char *value);
int ra_set_feedback_parameter_string(const char *key, const char *value);
/** 获取字符串类型参数
* @param[in]  key   参数名
* @param[out]  buf  缓冲区
* @param[in]  buf_len  缓冲区长度(包括结束符)
* @retval	非0     value实际长度
* @retval	-1      内部错误
* @retval	-3      参数错误
*/
int ra_get_parameter_string(const char *key, char *buf, ra_size_t buf_len);
int ra_get_feedback_parameter_string(const char *key, char *buf, ra_size_t buf_len);
/** 删除参数
* @param[in]  key   参数名
* @retval	0       获取成功
* @retval	-2      内存溢出
* @retval	-3       参数错误
*/
int ra_delete_parameter(const char *key);

#endif