
/* hfmsg.h
 *
 * Copyright (C) 2014-2015 ShangHai High-flying Electronics Technology Co.,Ltd.
 *
 * This file is part of HSF.
 * 
 * Modify:
 * 2015-10-10 : Create by Jim
 */

typedef enum t_HFEVENTMSG_ID 
{
	HF_MSG_WIFI_SCAN=1000,
	HF_MSG_STA_CONNECT_SCAN=1001,
	HF_MSG_STA_RECONNECT,
	HF_MSG_STA_CONNECT,
	HF_MSG_SOFTAP_START
}HFEVENTMSG_ID;

typedef struct t_HFEVENTMSG_DATA
{
	HFEVENTMSG_ID		msgid;
	int			    	data_len;
	void                *data;
} HFEVENTMSG_DATA ;


