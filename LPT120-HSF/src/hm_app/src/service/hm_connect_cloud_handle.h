#ifndef _HM_CONNECT_CLOUD_HANDLE_H_
#define _HM_CONNECT_CLOUD_HANDLE_H_

#include "hsf.h"

PROCESS_NAME(hm_connect_cloud_process);
void hm_start_connect_cloud(void);

typedef enum
{
    CC_START = 0,
    CC_GOT_INFO_SERVER_IP,
    CC_CONNECTED_INFO_SERVER,
    CC_RECEIVED_DEVICE_INFO,
    CC_SPECIFIED_SERVER_GET_IP,
    CC_SPECIFIED_SERVER_GOT_IP,
    CC_SPECIFIED_SERVER_CONNECTED,
    CC_SPECIFIED_SERVER_LOGIN_SUCCESS,
    CC_SPECIFIED_SERVER_LOGIN_FAIL,
    CC_SPECIFIED_SERVER_BIND_SUCCESS,
    CC_SPECIFIED_SERVER_BIND_FAIL,
    CC_SPECIFIED_SERVER_TOTAL_SUCCESS,
    CC_SPECIFIED_SERVER_TOTAL_FAIL,
    CC_SPECIFIED_SERVER_NORMAL_DATA_TRANSTER,
} cc_step_t;

typedef enum
{
    CC_STEP_TYPE,

} HM_CCP_MSG_TYPE;

typedef struct
{
    HM_CCP_MSG_TYPE msg_type;
    union 
    {
        cc_step_t cc_step;
    };
} HM_CCP_EVENT_MEG_T;

size_t cloud_creat_msgid(void);
int hm_tcp_send_data(char *data, unsigned short datalen);

void gatewaylogin_report(void);
void reportdevinfo_report(void);

void hm_cloud_heartbeat_timer_init(void);
void hm_cloud_heartbeat_timer_start(void);
void hm_cloud_heartbeat_timer_stop(void);

void hm_clear_heartbeat_miss_times(void);
void hm_add_heartbeat_miss_times(void);
uint8 hm_get_heartbeat_miss_times(void);
#endif