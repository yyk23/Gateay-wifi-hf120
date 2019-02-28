
/* hfsniffer.h
 *
 * Copyright (C) 2013-2014 ShangHai High-flying Electronics Technology Co.,Ltd.
 *
 * This file is part of HSF.
 * 
 * Modify:
 * 2013-12-25.
 */

#ifndef _HFSNIFFER_H_H_
#define _HFSNIFFER_H_H_
#include "hfwifi.h"


#define HFSNIFFER_RECE_TYPE_TO_ROUTER			0x01
#define HFSNIFFER_RECE_TYPE_FROM_ROUTER			0x02

typedef void (*hfsniffer_recv_callback_t)(uint8_t* /*pkt*/,uint32_t /*pkt_len*/);


#define HFSNIFFER_RECE_MODE_BEACON_MANAG		0x0001
#define HFSNIFFER_RECE_MODE_BROADCAST_DATA		0x0002

int HSF_API hfsniffer_start(uint8_t mode, hfsniffer_recv_callback_t cb);

int HSF_API hfsniffer_stop(void);

int HSF_API hfsniffer_set_filter(uint8_t mode);

int HSF_API hfsniffer_set_channel(uint8_t ch);

int HSF_API hfsniffer_is_run(void);


#endif


