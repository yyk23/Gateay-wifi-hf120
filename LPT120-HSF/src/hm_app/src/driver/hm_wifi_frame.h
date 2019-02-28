#ifndef __HM_WIFI_FRAME_H__
#define __HM_WIFI_FRAME_H_
#include "hsf.h"

int hm_tcp_frame_send(NETSOCKET socket_id, char *data, unsigned short datalen);
int hm_free_one_tcp_frame(void);
int hm_free_all_tcp_frame(void);
#endif