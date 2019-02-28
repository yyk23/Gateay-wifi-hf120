#ifndef _HM_DEV_MODEL_H_
#define _HM_DEV_MODEL_H_

#include "hsf.h"
#include "c_types.h"
#include "hm_uart_handle.h"
//设备类型定义

//水龙头
#define  WT_DEV_TYPE_NAME    "WT-1"

#define WT_DEV_MID    "M2kMb9GnIj2y"

typedef struct
{
     char *attr_name;
     char *datatype;
} attr_info_t;

typedef struct
{
    uint8 attr_posion;
    attr_info_t attr_info;
} attr_array_info_t;

typedef struct
{
    uint8 cmd_id;
    char *id_type;
    uint32 attr_num;
    char *attr_list;   
} cmd_info_t;

typedef struct hm_dev_model
{
    char *dev_name;
     char *dev_type;
     char *dev_factory;
    attr_array_info_t *attr_array;
    uint8 attrs_num;
    cmd_info_t *cmd_info;
    uint8 cmds_num;
}hm_dev_model_info_t;

hm_dev_model_info_t wt_dev_model_info;
int hm_dev_mode_init(void);
char *hm_get_dev_type(tid_t *tid);
cmd_info_t *hm_check_cmd_id(char *tdev_type, uint8 tcmd_id);
char *hm_get_attr_datatype(char *tdev_type, uint8 tposion);
char *hm_get_attr_name(char *tdev_type, uint8 tposion);
#endif