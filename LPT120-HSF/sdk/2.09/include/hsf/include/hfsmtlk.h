
/* hfsmtlk.h
 *
 * Copyright (C) 2013-2014 ShangHai High-flying Electronics Technology Co.,Ltd.
 *
 * This file is part of HSF.
 * 
 * Modify:
 * 2013-12-25 : Create by Jim
 */

#ifndef _HFSMTLK_H_H_
#define _HFSMTLK_H_H_
#include "hfwifi.h"

int HSF_API  hfsmtlk_init(void);


//#define  hfsmtlk_start()	hfat_send_cmd("AT+SMTLK\r\n", sizeof("AT+SMTLK\r\n")-1, NULL, 0)
int HSF_API  hfsmtlk_start(void);

#define HFSMTLK_FLAG_SET_TAG_ONLY			(0x01)
#define HFSMTLK_FLAG_SET_TAG_AND_RESET		(0x02)

int HSF_API  hfsmtlk_startex(uint32_t flags);

int HSF_API  hfsmtlk_stop(void);

int HSF_API  hfsmtlk_is_start(void);

int HSF_API hfsmtlk_enable_recv_data_from_router(int enable);

int HSF_API hfsmtlk_enable_recv_beacon(int enable);

typedef int (*hfsmtlk_recv_callback_t)( void * /*pkt_data*/,uint32_t /*pkt_length*/,uint8_t /*channel*/);

typedef int (*hfsmtlk_main_callback_t)( PWIFI_SCAN_RESULT_ITEM /*ap_list*/,uint32_t /*ap_cnt*/);

int HSF_API hfsmtlk_register(hfsmtlk_recv_callback_t recv_callback);

int HSF_API hfsmtlk_lock_channel(uint8_t *bssid, uint8_t ch);

#define HFSMTLK_FLAG_RECV_MGT_PKT			0x00000001
#define HFSMTLK_FLAG_CHNL_EXT_SCA			0x00010000
#define HFSMTLK_FLAG_CHNL_EXT_SCB			0x00020000

#define HFSMTLK_TYPE_HFV3					(0)
#define HFSMTLK_TYPE_HFV7					(1)
#define HFSMTLK_TYPE_AIRKISS				(2)
#define HFSMTLK_TYPE_USERDEFINE			(99)

int HSF_API hfsmtlk_set_filter(uint8_t channel,uint8_t rssi,uint32_t max_pkt_len,uint32_t min_pkt_len,uint32_t flags);

void HSF_API hfsmtlk_finished_ok(char *key, int key_len, uint8_t *apmac,char *ssid,uint8_t smtlk_type);

int HSF_API hfsmtlk_finished(uint8_t *ssid, int ssid_len, uint8_t *key, int key_len, uint8_t *bssid, uint8_t smtlk_type);

int HSF_API hfsmtlk_ap_done(uint8_t *ssid, uint8_t *key, uint8_t *bssid, enum ENWAAUTH auth, enum ENWAENCRY encry, uint8_t ch);

#endif


