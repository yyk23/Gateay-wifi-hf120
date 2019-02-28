#include "hsf.h"
#include "hm_dev_model.h"
#include "c_types.h"
#include "hm_uart_handle.h"
/***********************************
 *金牌 水龙头设备 模型定义
 * *********************************/

//属性定义
typedef enum
{
    WT_SW_ATTR_P = 0,
    WT_BATTERY_VALUE_ATTR_P,
} wt_attr_e;

//命令ID
typedef  enum
{
    WT_SW_CMD_ID = 0x01,
    WT_REPORT_STATUS_CMD_ID = 0x02,
}wt_cmd_e;

char wt_sw_cmd_attr_list[1] = {WT_SW_ATTR_P};
char wt_rep_status_cmd_attr_list[2] = {WT_SW_ATTR_P, WT_BATTERY_VALUE_ATTR_P};

attr_array_info_t wt_attr_array_info[] = {
    {WT_SW_ATTR_P, {"onoff", "INT"}},
    {WT_BATTERY_VALUE_ATTR_P, {"BT_value", "INT"}},
};

cmd_info_t wt_cmd_array[] = {
    {WT_SW_CMD_ID, "DOWN", NULL, NULL},
    {WT_REPORT_STATUS_CMD_ID, "UP",NULL, NULL},
};

hm_dev_model_info_t wt_dev_model_info = {
    "wt",
    WT_DEV_TYPE_NAME,
    "gold",
    NULL,
    0,
    NULL,
    0
};

/***********************************
 *毫米 开关精灵 模型定义  未定义
 * *********************************/




static cmd_array_init(void)
{
    wt_cmd_array[0].attr_num = sizeof(wt_sw_cmd_attr_list);
    wt_cmd_array[0].attr_list = wt_sw_cmd_attr_list;
    wt_cmd_array[1].attr_num = sizeof(wt_rep_status_cmd_attr_list);
    wt_cmd_array[1].attr_list = wt_rep_status_cmd_attr_list;
}

//设备模型注册
int hm_dev_mode_init(void)
{

    cmd_array_init();
    wt_dev_model_info.attr_array = wt_attr_array_info;
    wt_dev_model_info.attrs_num = (sizeof(wt_attr_array_info) / sizeof(attr_array_info_t));
    wt_dev_model_info.cmd_info = wt_cmd_array;
    wt_dev_model_info.cmds_num = (sizeof(wt_cmd_array) / sizeof(cmd_info_t));
}

char *hm_get_dev_type(tid_t *tid)
{

    return wt_dev_model_info.dev_type;

}

cmd_info_t *hm_check_cmd_id(char *tdev_type, uint8 tcmd_id)
{
    uint8 i = 0;

  //  u_printf("HM cmds  is %d  attrs num is %d", wt_dev_model_info.cmds_num, wt_dev_model_info.attrs_num);
    for (i = 0; i < wt_dev_model_info.cmds_num; i++)
    {
        if (wt_dev_model_info.cmd_info[i].cmd_id == tcmd_id)
            return &(wt_dev_model_info.cmd_info[i]);
    }

  return NULL;
}

char *hm_get_attr_name (char * tdev_type , uint8 tposion)
{
    uint8 i = 0;
    for (i = 0; i < wt_dev_model_info.attrs_num; i++)
    {
        if (wt_dev_model_info.attr_array[i].attr_posion == tposion)
            return wt_dev_model_info.attr_array[i].attr_info.attr_name;
    }

    return NULL;
}

char *hm_get_attr_datatype(char *tdev_type, uint8 tposion)
{
    uint8 i = 0;
    for (i = 0; i < wt_dev_model_info.attrs_num; i++)
    {
        if(wt_dev_model_info.attr_array[i].attr_posion == tposion)
            return wt_dev_model_info.attr_array[i].attr_info.datatype;
    }

    return NULL;
}