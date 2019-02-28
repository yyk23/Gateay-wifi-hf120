#ifndef _SOFT_AP_INIT_H
#define _SOFT_AP_INIT_H

#include <ssv_types.h>
#include <ssv_lib.h>
#include "uip_arp.h"

//void softap_set_gateway_and_client_iprange(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2);
S32 softap_init();
S32 softap_dhcp_init(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2,U8 gw_digit3);
S32 softap_init_ex(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2,U8 gw_digit3);
S32 softap_init_ex2(U8 gw_digit0,U8 gw_digit1,U8 gw_digit2,U8 gw_digit3,U8 max_sta_num,U8 encryt_mode, U8 keylen,U8 *key,U8 channel);

#endif //_SOFT_AP_INIT_H
