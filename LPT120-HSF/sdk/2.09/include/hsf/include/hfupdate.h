/* 
 *hferrno.h
 *
 * Copyright (C) 2013-2020 ShangHai High-flying Electronics Technology Co.,Ltd.
 *				  	All rights reserved.
 *
 * This file is part of HSF.
 *
 *
 */
#ifndef __HF_UPDATE__H__
#define __HF_UPDATE__H__


typedef enum HFUPDATE_TYPE
{
	HFUPDATE_SW=0,
	HFUPDATE_CONFIG=1,
	HFUPDATE_WIFIFW,
	HFUPDATE_WEB,
}HFUPDATE_TYPE_E;

int HSF_API hfupdate_start(HFUPDATE_TYPE_E type);

int HSF_API hfupdate_write_file(HFUPDATE_TYPE_E type ,uint32_t offset,uint8_t *data,int len);

int HSF_API hfupdate_complete(HFUPDATE_TYPE_E type,uint32_t file_total_len);

int HSF_API hfupdate_auto_upgrade(uint32_t flags);

int HSF_IAPI hfupdate_auto_upgrade_default(void);

int HSF_IAPI hfupdate_boot(uint32_t boot_type);

int HSF_API hfupdate_auto_upgrade_start(void);

int HSF_API hfupdate_auto_upgrade_is_running();

int HSF_API hfupdate_mcu_ota_valid(int *file_len, int *check_sum32);

int HSF_API hfupdate_mcu_ota_data_read(int offset, unsigned char *data, int len);

int HSF_API hfupdate_mcu_ota_clear(void);

#endif




