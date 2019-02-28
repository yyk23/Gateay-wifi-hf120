/*
 * airkiss.h
 *
 *  Created on: 2015-1-26
 *      Author: peterfan
 */

#ifndef AIRKISS_H_
#define AIRKISS_H_

#include "ssv_types.h"
#include "smartComm.h"

#if (defined(SMART_WECHAT) ||  defined(SMART_USER))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AIRKISS_ENABLE_CRYPT
#define AIRKISS_ENABLE_CRYPT 0
#endif

#define AIRKISS_SCAN_PERIOD 150 * CLOCK_MINI_SECOND

typedef void* (*airkiss_memset_fn) (void* ptr, int value, unsigned int num);
typedef void* (*airkiss_memcpy_fn) (void* dst, const void* src, unsigned int num);
typedef int (*airkiss_memcmp_fn) (const void* ptr1, const void* ptr2, unsigned int num);
typedef int (*airkiss_printf_fn) (const char* format, ...);

typedef struct
{
    airkiss_memset_fn memset;
    airkiss_memcpy_fn memcpy;
    airkiss_memcmp_fn memcmp;
    airkiss_printf_fn printf;
} airkiss_config_t;

typedef struct
{
    int dummy[32];
} airkiss_context_t;

typedef struct
{
	char* pwd;
	char* ssid;	
	unsigned char pwd_length;
	unsigned char ssid_length;
	unsigned char random;
	unsigned char reserved;
} airkiss_result_t;

typedef enum
{
    AIRKISS_STATUS_CONTINUE = 0,
    AIRKISS_STATUS_CHANNEL_LOCKED = 1,
    AIRKISS_STATUS_COMPLETE = 2
} airkiss_status_t;

#if AIRKISS_ENABLE_CRYPT
int airkiss_set_key(airkiss_context_t* context, const unsigned char* key, unsigned int length);
#endif

const char* airkiss_version();

int airkiss_init(airkiss_context_t* context, const airkiss_config_t* config);

int inline airkiss_recv(airkiss_context_t* context, const void* frame, unsigned short length) ATTRIBUTE_SECTION_SRAM;

int airkiss_get_result(airkiss_context_t* context, airkiss_result_t* result);

void airKissResult();

void clearAirKissBuf();
void initAirKiss();

airkiss_status_t inline airKissRecv(void *frame, short len) ATTRIBUTE_SECTION_SRAM;

int airKissInit();

void airKissGetVersion();
void rx_process_airkiss() ATTRIBUTE_SECTION_SRAM;

#ifdef __cplusplus
}
#endif

#endif

#endif /* AIRKISS_H_ */
