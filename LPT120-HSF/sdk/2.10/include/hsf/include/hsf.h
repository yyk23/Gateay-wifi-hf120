
/* hsf.h
 *
 * Copyright (C) 2013 ShangHai High-flying Electronics Technology Co.,Ltd.
 *
 * This file is part of HSF.
 *
 */

#ifndef _HSF_H_H_H_H_H_
#define _HSF_H_H_H_H_H_

#include "contiki.h"
#include "contiki-net.h"

#include "hftypes.h"
#include "hfconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLOBAL_VER 				"2.0.10"
#define GLOBAL_LVER				"7 (2018-04-24 1636)"


#define USER_FUNC
#define HSF_API
#define HSF_IAPI

#define ATTRIBUTE_SECTION_KEEP_IN_SRAM  __attribute__((section("keep_in_sram")))


#define  HFM_LPT120			(0)
#define  HFM_LPB120			(1)
#define  HFM_LPT220			(2)
#define  HFM_LPB125			(3)
#define  HFM_SIP120			(4)
#define  HFM_LPT120G		(5)

#define _HSF_INLINE_	inline

#define SMTLK_USER_INFO_IN_PASSWORD

#include "hferrno.h"
#include "hfat.h"
#include "hfgpio.h"
#include "hfuart.h"
#include "hftimer.h"
#include "hfupdate.h"
#include "hfflash.h"
#include "hfwifi.h"
#include "hfsys.h"
#include "hfdebug.h"
#include "hfnet.h"
#include "hfmsg.h"
#include "hfwifi.h"
#include "hfhttpc.h"
#include "hfsmtlk.h"
#include "hffile.h"
#include "hfsniffer.h"

#endif


