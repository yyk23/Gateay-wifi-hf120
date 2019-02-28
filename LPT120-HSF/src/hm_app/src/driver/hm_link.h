#ifndef _HM_LINK_H
#define _HM_LINK_H

typedef enum
{
    HM_LINK_STA_MODE = 0,
    HM_LINK_AP_MODE,
    HM_LINK_SUCCESS,
    HM_LINK_FAIL,
} HM_LINK_MODE;

typedef enum
{
    HM_LINK_NONE= 0,
    HM_LINK_OK,
} HM_LINK_STATUS;

#define HM_LINK_START_MODE_DEFAULT  HM_LINK_AP_MODE

//配网时间100秒
#define HM_LINK_TIMEOUT_TIMES       100

int  ghm_start_link(void);

HM_LINK_STATUS check_link_state(void);

void hm_connect_wifi(char *ssidq, char *pswd);
void hm_set_ap_mode(void);
#endif