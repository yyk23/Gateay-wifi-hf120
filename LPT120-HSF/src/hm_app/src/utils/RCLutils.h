#ifndef __RCLUTILS_H__
#define __RCLUTILS_H__
#include "hsf.h"
#define HttpSymbolNUM	20


/*string struct define*/
typedef enum
{
	DYNAMIC_PARAM_TYPE_NULL = 0,
	DYNAMIC_PARAM_TYPE_RAW,
	DYNAMIC_PARAM_TYPE_STRING,
	DYNAMIC_PARAM_TYPE_INT,
}dynamic_param_type_t;

typedef struct dynamic_param_s {
	dynamic_param_type_t type;
	union
	{
		int32_t value_int;
		struct
		{
			char *src;
			size_t len;
		}value_str;
		struct
		{
			uint8_t *src;
			size_t len;
		}value_raw;
	}u;
}dynamic_param_t;
/*string struct end define*/



void bitmap_set(size_t *bitmap, size_t num, uint8_t value);

size_t bitmap_forech(size_t *bitmap, size_t bitmap_length, size_t startnum, uint8_t value);

size_t lib_strnlen(const char *s, size_t maxlen);
int lib_atoi(char s[]);

unsigned char is_symbol(char c);

unsigned char is_number(char c);

unsigned char is_space(char c);

uint32_t lib_strtohex(uint8_t* des, char *src, uint32_t strlen);

void  lib_hextostr(char* des, uint8_t *src, uint32_t datalen, bool _case);

void  lib_strtostr(char* des, uint8_t *src, uint32_t datalen, bool _case);

char* str_clone(const char *src, size_t strlen);

char* get_http_head_value(char *httpdata, const char *target);

unsigned char str_is_number(char *s, uint32_t strlen);

float kalman_filter(const float src_data, float process_niose_q, float measure_noise_r);

bool get_ip_from_str(uint32_t *ip,const char *str);

bool get_str_from_str(
	char *src,
	const char*match_head,
	const char*match_tail,
	char **des_str,
	char **remainder_str);

void dynamic_param_destory(dynamic_param_t *p);

void dynamic_param_type_convert(dynamic_param_t *p, dynamic_param_type_t type);

dynamic_param_t *dynamic_param_new(dynamic_param_type_t type, const void *src, size_t size);

char *dynstring_append(char *des, char *src);

char *dynstring_append_len(char *des, size_t desStrlen, char *src, size_t srcStrlen);

char *dynstring_init(void);

void * lib_memchr(const void * buffer, int ch, int count);
#endif