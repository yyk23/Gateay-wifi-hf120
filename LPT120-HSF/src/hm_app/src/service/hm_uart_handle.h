#ifndef _HM_UART_HANDLE_H_
#define _HM_UART_HANDLE_H_

#include "hsf.h"
#define HM_UART_SEND_MAX_BUF_LEN  200

#define MAX_TID_BYTES_LENTH    8
#define MAX_MID_BYTES_LENTH    12

#define TID_BYTES_LENTH       8
#define MID_BYTES_LENTH       12

typedef struct 
{
    uint8 seq_num;
}hm_48_frame_inf_t;



typedef struct at_cmd
{
    const char *name;
    int (*func)(char *); 
} hm_at_cmd_t, *phm_at_cmd_t;

typedef struct
{
    uint8 len;
    char data[MAX_TID_BYTES_LENTH];
} tid_t;

typedef struct 
{
    char *dev_type;
    tid_t tid; //设备TID地址
    uint8 cmd_id;     //命令ID
    uint8 attrs_lenth; //属性数据总长度
    char *attrs_data;  //属性数据指针
} hm_48_valid_data_t;


typedef struct
{
    uint8 len;
    char  data[MAX_MID_BYTES_LENTH];
} mid_t;

extern char shm_uart_send_buffer[HM_UART_SEND_MAX_BUF_LEN];
PROCESS_NAME(hm_uart_handle_process);

int hm_send_start_at_factor_test(void) ;
int shm_atcmd_star(char *str);
int shm_atcmd_set_prodkey(char *str);
int shm_atcmd_set_devtid(char *str);
int shm_atcmd_set_devprikey(char *str);
int shm_atcmd_wifi_test(char *str);
int shm_atcmd_clear_all_data(char *str);
//添加子设备发送函数
int hm_48_add_subdev_frame_send(mid_t *tmid);
//删除子设备发送函数
int hm_48_delete_subdev_frame_send(tid_t *ttid);
//下发数据到子设备发送函数
int hm_48_app_to_subdev_frame_send(hm_48_valid_data_t *tvalid_data);
void hm_48_repond_data_error_frame(void);
#endif