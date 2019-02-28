#include "ssv_lib.h"
#include "ssv_types.h"

#ifndef __ADAPTIVE_PROC_H__
#define __ADAPTIVE_PROC_H__

typedef enum t_ENANGY_THRESHOLD
{
	MINUS_88DB 		= 0,
	MINUS_85DB,
	MINUS_82DB,
	MINUS_79DB,
    MINUS_76DB,
    MINUS_73DB,
    MINUS_72DB,
} ENANGY_THRESHOLD;

typedef struct t_ADAPTIVITY_PARAM
{
        U32  nInterval;      //ms second
        U32  nLimitCount;
        U32  nTotalCount;
        U32 bShowDebug;
} ADAPTIVITY_PARAM;

void adaptive_process_start(ENANGY_THRESHOLD threshold);

#endif

