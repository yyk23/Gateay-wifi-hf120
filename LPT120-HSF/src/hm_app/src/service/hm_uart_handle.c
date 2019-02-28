#include "hsf.h"
#include "hm_parameter.h"
#include "hm_system.h"
#include "hm_test.h"
#include "hm_uart.h"
#include "hm_uart_handle.h"
#include "hm_sub_devsend_handle.h"
#include "hm_dev_model.h"
#include "hm_connect_cloud_handle.h"
PROCESS(hm_uart_handle_process, "hm_uart_handle_process");
static int hm_48_frame_handle(char *data, uint32 len);
static void shm_control_wifimod_handle(char *payload, uint32 len);
static void hm_48_repond_error_frame(char code);
static void shm_uart_get_status_resp(void);
static int shm_48_send_frame(char *data, uint32 len);
static void shm_at_handle(char *str, uint32 len);
static int shm_48_control_gateway_handle(char *data, uint32 len) ;
static int shm_48_subdev_report_handle(char *data, uint32 len);


#define PAYLOAD_DATA_TYPE_LEN 1
#define PAYLOAD_DATA_TYPE_CONTROL_PRODUCT 0x30
#define PAYLOAD_DATA_TYPE_WIFIMOD_STATE 0xFD
#define PAYLOAD_DATA_TYPE_PRODUCT_STATE_REPORT 0x01 //从机主动上报 与 wifi模块应答帧 复用
#define PAYLOAD_DATA_TYPE_APPSEND_RESP 0x02         //控制状态返回
#define PAYLOAD_DATA_TYPE_HM_GATEWAY_CONTROL_REPORT   0x03
#define PAYLOAD_DATA_TYPE_HM_APP_TO_GATEWAY_CONTROL 0x04
#define PAYLOAD_DATA_TYPE_PRODUCT_PAYINTERFACE 0x11

#define PAYLOAD_DATA_TYPE_HM_SUBDEV_REPORT 0x21
#define PAYLOAD_DATA_TYPE_HM_APP_TO_SUBDEV 0x22

#define PAYLOAD_DATA_TYPE_MCU_WIFIMOD 0xEF
#define PAYLOAD_DATA_TYPE_CONTROL_WIFIMOD 0xFE
#define PAYLOAD_DATA_TYPE_ERROR 0xFF

#define PAYLOAD_DATA_ERROR_CODE_INTERNAL_ERROR 0x01  //模块内部错误
#define PAYLOAD_DATA_ERROR_CODE_CHECK_FAIL 0x02      //帧校验错误
#define PAYLOAD_DATA_ERROR_CODE_FORMAT_ERROR 0x03    //帧格式错误
#define PAYLOAD_DATA_ERROR_CODE_COMMAND_INVALID 0x04 //帧命令不支持
#define PAYLOAD_DATA_ERROR_CODE_DATA_ERROR 0x05      //帧数据与协议模板中数据不匹配

#define GET_PAYLOAD_TYPE 0
#define GET_PAYLOAD_SERIAL_NUMBER 1
#define GET_PAYLOAD_COMMAND 2

/*mcu wifi mod command*/
#define MCU_WIFI_MOD_READ_VERSION 0X01
#define MCU_WIFI_MOD_PUSH_UPGRADE 0X02
#define MCU_WIFI_MOD_GET_DATA_LEN 0X11
#define MCU_WIFI_MOD_GET_DATA 0X12
#define MCU_WIFI_MOD_GET_COMPLETED 0X13
#define MCU_WIFI_MOD_UPGRADE_COMPLETED 0X14

/*control gateway  03*/

#define CONTROL_GATEWAY_CMMAND_ADD_SUBDEV_SUCCESS  0x01
#define CONTROL_GATEWAY_CMMAND_SUBDEV_STATUS_REPORT 0x02


/*app to control gateway cmmand 04*/
#define APP_TO_GATEWAY_CONTROL_CMMAND_ADD_SUBDEV_REQ   0x01
#define APP_TO_GATEWAY_CONTROL_CMMAND_DELETE_SUBDEV_REQ 0x02



/*contol wifi mod command*/
#define CONTROL_WIFI_MOD_COMMAND_GET_STATUS 0x01
#define CONTROL_WIFI_MOD_COMMAND_REBOOT 0x02
#define CONTROL_WIFI_MOD_COMMAND_CONFIG_RESET 0x03
#define CONTROL_WIFI_MOD_COMMAND_HEKR_CONFIG 0x04
#define CONTROL_WIFI_MOD_COMMAND_MODEM_SLEEP 0x05
#define CONTROL_WIFI_MOD_COMMAND_WAKEUP 0x06
#define CONTROL_WIFI_MOD_COMMAND_SET_BAUDRATE 0x07
#define CONTROL_WIFI_MOD_COMMAND_SET_NETSTATUSLEDMOD 0x08
#define CONTROL_WIFI_MOD_COMMAND_GET_BIN_VERSION 0x10
#define CONTROL_WIFI_MOD_COMMAND_CHECK_PRODKEY 0x11
#define CONTROL_WIFI_MOD_COMMAND_SCANWIFI_SSID 0x12
#define CONTROL_WIFI_MOD_COMMAND_SET_SSID_PASSWORD 0x13
#define CONTROL_WIFI_MOD_COMMAND_SET_TEST_FLAG 0x20
#define CONTROL_WIFI_MOD_COMMAND_SET_PRODKEY 0x21
#define CONTROL_WIFI_MOD_COMMAND_SET_WECHATID 0x22
#define CONTROL_WIFI_MOD_COMMAND_SET_WECHATPRODUCTID 0x23
#define CONTROL_WIFI_MOD_COMMAND_RESET_AIRKISS 0x24
#define CONTROL_WIFI_MOD_COMMAND_RESET_HEKR_CONFIG 0x25
#define CONTROL_WIFI_MOD_COMMAND_CONFIG_CLEAN_TOKEN_RESET 0x30

/*wifi test mode*/
#define CONTROL_WIFI_MOD_COMMAND_BEGIN_AUTHORIZATION 0x50
#define CONTROL_WIFI_MOD_COMMAND_SET_AUTHSSID 0x51
#define CONTROL_WIFI_MOD_COMMAND_SET_GPIO_TEST 0x52
#define CONTROL_WIFI_MOD_COMMAND_GET_DEVTID 0x53
#define CONTROL_WIFI_MOD_COMMAND_SET_AUTHKEY 0x54
#define CONTROL_WIFI_MOD_COMMAND_CLEAR_AUTHKEY 0x55
#define CONTROL_WIFI_MOD_COMMAND_GET_AUTHKEY 0x56
#define CONTROL_WIFI_MOD_COMMAND_SET_OLD_FACTORYTEST 0x57
#define CONTROL_WIFI_MOD_COMMAND_SET_AUTH_PRODKEY 0x58
#define CONTROL_WIFI_MOD_COMMAND_IS_NEW_FACTORYTEST_SUCCESS 0x59

/*internal command*/
#define CONTROL_WIFI_MOD_COMMAND_GET_PRODKEY 0x31
#define CONTROL_WIFI_MOD_COMMAND_GET_DEV_TID 0x32
#define CONTROL_WIFI_MOD_COMMAND_SET_LOG_PORT 0x33
/*end*/

/*set baudrate*/
#define SET_BAUDRATE_9600 1
#define SET_BAUDRATE_19200 2
#define SET_BAUDRATE_38400 3
#define SET_BAUDRATE_43000 4
#define SET_BAUDRATE_56000 5
#define SET_BAUDRATE_57600 6
#define SET_BAUDRATE_74880 7
#define SET_BAUDRATE_115200 8
#define SET_BAUDRATE_230400 9
/*send to MCU command*/
#define CONTROL_WIFI_MOD_COMMAND_SEND_NTP_TIME 0x40

#define CONTROL_WIFI_MOD_COMMAND_RETURN_OK 0

#define RESERVE_BYTE 0

#define FACTORY_SCAN_OK 1
#define FACTORY_SCAN_FAIL 0

    char scan_ssid[64];

/*contol wifi mod command. config reset reserve byte*/
#define RESERVER_BYTE_WITH_CLEAR_TOKEN 0x01
char shm_uart_send_buffer[HM_UART_SEND_MAX_BUF_LEN];
hm_48_frame_inf_t hm_48_frame_inf;
uint8 send_seq_num = 0;

PROCESS_THREAD(hm_uart_handle_process, ev, data)
{
    PROCESS_BEGIN();
    HM_UART_EVENT_MEG_T tmsg;
    while (1)
    {

        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_CONTINUE);
        if (ev == PROCESS_EVENT_MSG)
        {
            tmsg = *(HM_UART_EVENT_MEG_T *)data;
            //48命令解析
            if (tmsg.msg_type == UART_48_TYPE)
            {
                u_printf("HM UART HANDLE  MESG RECEiVED ! ! ! \r\n");
                u_printf("HM UART HANDLE msg type is %d \r\n",tmsg.msg_type);
                u_printf("HM UART HANDLE msg lenth is %d \r\n", tmsg.msg_lenth);
                char * pp = hex_to_str(tmsg.msg_data, tmsg.msg_lenth , false);
                u_printf("[%s] \r\n" , pp);
                __FREE(pp);

                hm_48_frame_handle(tmsg.msg_data, tmsg.msg_lenth);
            }
            //AT厂测命令解析
            else if (tmsg.msg_type == UART_AT_TYPE)
            {
                u_printf("HM UART HANDLE AT MSG : %s",tmsg.msg_data);
                shm_at_handle(tmsg.msg_data, tmsg.msg_lenth);
            }
            __FREE(tmsg.msg_data);
        }
    }
    PROCESS_END();
}

static int hm_48_frame_handle(char * data, uint32 len)
{
    hm_48_frame_inf.seq_num = data[3];
    /*frame type*/
    switch (data[2])
    {
        case PAYLOAD_DATA_TYPE_PRODUCT_STATE_REPORT:
        {
            u_printf("HM UART PRODUCT_STATE_REPORT");
            //回复命令
            memcpy(shm_uart_send_buffer, data, len);
            hm_uart_send_frame_to_device(shm_uart_send_buffer, len);
            //进行JSON转换
            break;
        }
        case PAYLOAD_DATA_TYPE_APPSEND_RESP:
        {
            //回执
            u_printf("HM UART APPSEND RESP");
            break;
        }
        case PAYLOAD_DATA_TYPE_HM_GATEWAY_CONTROL_REPORT:
        {
            u_printf("HM UART HM_GATEWAY_CONTROL_REPORT");   
            if(shm_48_control_gateway_handle(data + UART_48_FRAME_HEAD_LENTH, len - UART_48_FRAME_HEAD_LENTH) ==0)
            {
                memcpy(shm_uart_send_buffer, data, len);
                hm_uart_send_frame_to_device(shm_uart_send_buffer, len);
            }
            else
            {
                hm_48_repond_data_error_frame();
            }
            
            break;
        }
        case PAYLOAD_DATA_TYPE_HM_APP_TO_GATEWAY_CONTROL:
        {
            u_printf("HM UART PAYLOAD_DATA_TYPE_HM_APP_TO_GATEWAY_CONTROL");
            break;
        }
        case PAYLOAD_DATA_TYPE_HM_SUBDEV_REPORT:
        {
           // hm_sub_devsend_report(hm_48_valid_data_t * valid_data)
            memcpy(shm_uart_send_buffer, data, len);
            if(shm_48_subdev_report_handle(data + UART_48_FRAME_HEAD_LENTH, len - UART_48_FRAME_HEAD_LENTH) == 0)
            {
                memcpy(shm_uart_send_buffer, data, len);
                hm_uart_send_frame_to_device(shm_uart_send_buffer, len);
            }
            else
            {
                hm_48_repond_data_error_frame();
            }
            
            break;
        }
        case PAYLOAD_DATA_TYPE_HM_APP_TO_SUBDEV:
        {
            //回执
            u_printf("HM UART APP TO SUBDEV");
            break;
        }
        //reverse
        case PAYLOAD_DATA_TYPE_PRODUCT_PAYINTERFACE:
        {
            break;
        }
      //reverse
        case PAYLOAD_DATA_TYPE_MCU_WIFIMOD:
        {
            break;
        }

        case PAYLOAD_DATA_TYPE_CONTROL_WIFIMOD:
        {
            u_printf("HM UART CONTROL_WIFIMOD");
            shm_control_wifimod_handle(data + UART_48_FRAME_HEAD_LENTH, len - UART_48_FRAME_HEAD_LENTH);
            break;
        }
        case PAYLOAD_DATA_TYPE_ERROR:
        {
            break;
        }
        default:
        {

            break;
        }
    }
    return 0;
}

static void shm_at_handle(char * str, uint32 len)
{
    const hm_at_cmd_t thm_define_at_cmds_table[] =
        {
            {"AT+HKBC", shm_atcmd_star},
            {"+HKPK",shm_atcmd_set_prodkey},
            {"+HKDT",shm_atcmd_set_devtid},
            {"+HKDK",shm_atcmd_set_devprikey},
            {"+HKWT",shm_atcmd_wifi_test},
            {"+HKCD" ,shm_atcmd_clear_all_data},
            {NULL, NULL}
        };
    uint8 i = 0;
    for (i = 0; i < sizeof(thm_define_at_cmds_table) / sizeof(hm_at_cmd_t);i++)
    {
        if (!memcmp(thm_define_at_cmds_table[i].name,str , strlen(thm_define_at_cmds_table[i].name)))
        {
            u_printf("HM UART HANDLE AT is %d", i);

            thm_define_at_cmds_table[i].func(strchr(str, '=')+1);
        }
    }
}

int hm_send_start_at_factor_test(void)
{
    size_t len = sprintf(shm_uart_send_buffer, "+HKBC\r\n");
    hm_uart_send_frame_to_device(shm_uart_send_buffer, len);
    return 0;
}

int shm_atcmd_star(char *str)
{
    //停止发送 "+HKBC"
    if ((*str) == '1')
    {
        hm_sys_run_info.run_info.FACTORY_TEST_STATUS = FACTORY_TEST_START;    
    }
    return 0;
}

int shm_atcmd_set_prodkey(char *str)
{
    uint32 len = strchr(str, ',') - str ;
    char *pprodkey = hfmem_malloc(len +1);
    size_t len1 = 0;
    memcpy(pprodkey, str,len);
    pprodkey[len] = '\0';
    if (product_info_set_prodkey(pprodkey) == 0)
    {
        //发送成功命令
        len1 = sprintf(shm_uart_send_buffer, "AT+HKPK=1\r\n");
    }
    else
    {
        len1 = sprintf(shm_uart_send_buffer, "AT+HKPK=2\r\n");
    }
    hm_uart_send_frame_to_device(shm_uart_send_buffer, len1);
    __FREE(pprodkey);
    return 0;
}


int shm_atcmd_set_devtid(char *str)
{
    size_t len1 = 0;
    uint32 len = strchr(str, ',') - str;
    char *pdevtid = hfmem_malloc(len + 1 );
    memcpy(pdevtid, str, len);
    pdevtid[len] = '\0';
    if (product_info_set_devtid(pdevtid) == 0)
    {
        //发送成功命令
        len1 = sprintf(shm_uart_send_buffer, "AT+HKDT=1\r\n");
    }
    else
    {
        len1 = sprintf(shm_uart_send_buffer, "AT+HKDT=2\r\n");
    }
    hm_uart_send_frame_to_device(shm_uart_send_buffer, len1);
    __FREE(pdevtid);
    return 0;
}
int shm_atcmd_set_devprikey(char *str)
{
    size_t len1 = 0;
    uint32 len = strchr(str, ',') - str;
    char *pdevprikey = hfmem_malloc(len + 1);
    memcpy(pdevprikey, str, len);
    pdevprikey[len] = '\0';
    if (product_info_set_devprikey(pdevprikey) == 0)
    {
        //发送成功命令
        len1 = sprintf(shm_uart_send_buffer, "AT+HKDK=1\r\n");
    }
    else
    {
        len1 = sprintf(shm_uart_send_buffer, "AT+HKDK=2\r\n");
    }
    __FREE(pdevprikey);
    hm_uart_send_frame_to_device(shm_uart_send_buffer, len1);
    hm_sys_run_info.run_info.FACTORY_TEST_STATUS = FACTORY_TEST_SUCCESS;
    return 0;
}

int shm_atcmd_wifi_test(char *str)
{
    char rsp[64];
    size_t len1 = 0;
    uint32 ssid_len = strchr(str, ',') - str;
    uint32 spassword_len = strchr(str, '\r') - (strchr(str, ',') +1);
    char *pssid = hfmem_malloc(ssid_len + 1);
    char *ppassword = hfmem_malloc(spassword_len + 1);
    memcpy(pssid, str, ssid_len);
    memcpy(ppassword, strchr(str, ',')+1 , spassword_len);
    pssid[ssid_len] = '\0';
    ppassword[spassword_len] = '\0'; 
    hm_connect_wifi(pssid, ppassword);
  //保存设备状态

    len1 = sprintf(shm_uart_send_buffer, "AT+HKWT=1\r\n");
    hm_uart_send_frame_to_device(shm_uart_send_buffer, len1);
    __FREE(pssid);
    __FREE(ppassword);
    product_info_set_factory_test(WIFI_TEST_FLAG);
    //重启
    hfat_send_cmd("AT+Z\r\n", sizeof("AT+Z\r\n"), rsp, 64);
    return 0;
}

int shm_atcmd_clear_all_data(char *str)
{
    //"1" 只清除厂测等数据 
    //"2"清除 厂测数据以外的数据
    //"3"清除所有保存数据

    size_t len1 = 0;
    switch (str[0])
    {
        case '1':
            hm_restory_factory_data(&product_info);
            len1 = sprintf(shm_uart_send_buffer, "AT+HKCD=1\r\n");
            break;
        case '2':
            hm_restory_app_data(&product_info);
            len1 = sprintf(shm_uart_send_buffer, "AT+HKCD=1\r\n");
            break;
        case '3':
            hm_restory_all_data(&product_info);
            len1 = sprintf(shm_uart_send_buffer, "AT+HKCD=1\r\n");
            break;
        default:
            len1 = sprintf(shm_uart_send_buffer, "AT+HKCD=2\r\n");
            break;
    }
    hm_uart_send_frame_to_device(shm_uart_send_buffer, len1);
    return 0;
}
static void shm_control_wifimod_handle(char *payload, uint32 len)
{
    switch (payload[0])
    {
        /*get status*/
        case CONTROL_WIFI_MOD_COMMAND_GET_STATUS:
        {
            shm_uart_get_status_resp();
             
            break;
        }

        /*reboot*/
        case CONTROL_WIFI_MOD_COMMAND_REBOOT:
        {
            char ret[] = {
                PAYLOAD_DATA_TYPE_CONTROL_WIFIMOD,
                hm_48_frame_inf.seq_num,
                CONTROL_WIFI_MOD_COMMAND_REBOOT,
                CONTROL_WIFI_MOD_COMMAND_RETURN_OK};

            shm_48_send_frame(ret, sizeof(ret));
            //系统软重启
            hfsys_softreset();
            break;
        }
        /*config reset*/
        case CONTROL_WIFI_MOD_COMMAND_CONFIG_RESET:
        {
            uint8 reserve_byte = payload[1];

            char ret[] = {
                PAYLOAD_DATA_TYPE_CONTROL_WIFIMOD,
                hm_48_frame_inf.seq_num,
                CONTROL_WIFI_MOD_COMMAND_CONFIG_RESET,
                reserve_byte};

            shm_48_send_frame(ret, sizeof(ret));
            if (hm_sys_run_info.run_info.run_status == RUN_CLOUD_LINK_STATUS || hm_sys_run_info.run_info.run_status == RUN_NORMAL_STATUS)
            {
                process_post_synch(&hm_connect_cloud_process, PROCESS_EVENT_EXIT, NULL);
            }

            product_info_set_factory_test(WIFI_CONFIG_FLAG);
            hm_set_ap_mode(); //设置AP模式 并重启
            break;
        }
        default:
        {
            break;
        }
    }
}

static int   shm_48_control_gateway_handle(char * data, uint32 len)
{
    //子设备添加成功
    //子设备状态上报
    tid_t ttid;
    mid_t tmid;
    uint8 tonline_status = 0;
    switch (data[0])
    {
        case CONTROL_GATEWAY_CMMAND_ADD_SUBDEV_SUCCESS:
        {
            ttid.len = TID_BYTES_LENTH;
            memcpy(ttid.data, data + 1, TID_BYTES_LENTH);
            tmid.len = MID_BYTES_LENTH;
            memcpy(tmid.data, data + 1 + TID_BYTES_LENTH, MID_BYTES_LENTH);
            tonline_status = *(data + 1 + TID_BYTES_LENTH + MID_BYTES_LENTH);
            hm_add_sub_dev_report(&ttid, &tmid, tonline_status);
            break;
        }
        case CONTROL_GATEWAY_CMMAND_SUBDEV_STATUS_REPORT:
        {
            ttid.len = TID_BYTES_LENTH;
            memcpy(ttid.data, data + 1, TID_BYTES_LENTH);
            tmid.len = MID_BYTES_LENTH;
            memcpy(tmid.data, data + 1 + TID_BYTES_LENTH, MID_BYTES_LENTH);
            tonline_status = *(data + 1 + TID_BYTES_LENTH + MID_BYTES_LENTH);
            hm_sub_dev_status_report(&ttid, &tmid, tonline_status);
            break;
        }
        default:
        {
            return 1;
            break;
        }
    }
    return 0;

}



static int shm_48_subdev_report_handle(char *data, uint32 len)
{
    hm_48_valid_data_t valid_data;

    valid_data.cmd_id = data[0];
    valid_data.tid.len = TID_BYTES_LENTH;
    memcpy(valid_data.tid.data, data + 1, TID_BYTES_LENTH);
    valid_data.attrs_lenth = len - 1 - valid_data.tid.len;
    valid_data.attrs_data = data + 1 + valid_data.tid.len;
    valid_data.dev_type = hm_get_dev_type(&valid_data.tid);
    u_printf("HM shm_48_subdev_report_handle dev type is %s", hm_get_dev_type(&valid_data.tid));
    return hm_sub_devsend_report(&valid_data);
    
}

static void hm_48_repond_error_frame(char code)
{
    char error_code[] = {
        PAYLOAD_DATA_TYPE_ERROR,
        hm_48_frame_inf.seq_num,
        code,
        RESERVE_BYTE};
    shm_48_send_frame(error_code, sizeof(error_code));
}


void hm_48_repond_data_error_frame(void)
{
    char error_code[] = {
        PAYLOAD_DATA_TYPE_ERROR,
        hm_48_frame_inf.seq_num,
        PAYLOAD_DATA_ERROR_CODE_DATA_ERROR,
        RESERVE_BYTE};
    shm_48_send_frame(error_code, sizeof(error_code));
}


static void shm_uart_get_status_resp(void)
{
    char shelf_status[] = {
        PAYLOAD_DATA_TYPE_CONTROL_WIFIMOD,
        hm_48_frame_inf.seq_num,
        CONTROL_WIFI_MOD_COMMAND_GET_STATUS,
        hm_get_wifi_mode(),
        hm_get_wifi_status(),
        hm_get_cloud_connect_status(),
        hm_get_signal_qual(),
        hm_get_device_test_status()};

    shm_48_send_frame(shelf_status, sizeof(shelf_status));
}


static uint8  shm_get_48_frame_seq(void)
{
    return send_seq_num++;
}

static int shm_48_send_frame(char * data ,uint32 len)
{
    uint8 i = 0;
    shm_uart_send_buffer[i++] = UART_48_FRAME_HEAD_NUM;
    shm_uart_send_buffer[i++] = len+3;
    memcpy(&shm_uart_send_buffer[i], data, len);
    shm_uart_send_buffer[i + len] = hm_48_frame_sum_Calculate(shm_uart_send_buffer, len+2);
    return hm_uart_send_frame_to_device(shm_uart_send_buffer, len+3);
}

//添加子设备发送函数
int hm_48_add_subdev_frame_send(mid_t * tmid)
{
    char *data = NULL;
    data = hfmem_malloc(3 + tmid->len);
    data[0] = PAYLOAD_DATA_TYPE_HM_APP_TO_GATEWAY_CONTROL;
    data[1] = shm_get_48_frame_seq();
    data[2] = APP_TO_GATEWAY_CONTROL_CMMAND_ADD_SUBDEV_REQ;
    memcpy(data +3 , tmid->data , tmid->len);
    shm_48_send_frame(data, 3 + tmid->len);
    __FREE(data);
    return 0;
}

//删除子设备发送函数
int hm_48_delete_subdev_frame_send(tid_t *ttid)
{
    char *data = NULL;
    data = hfmem_malloc(3 + ttid->len);
    data[0] = PAYLOAD_DATA_TYPE_HM_APP_TO_GATEWAY_CONTROL;
    data[1] = shm_get_48_frame_seq();
    data[2] = APP_TO_GATEWAY_CONTROL_CMMAND_DELETE_SUBDEV_REQ;
    memcpy(data + 3, ttid->data, ttid->len);
    shm_48_send_frame(data, 3 + ttid->len);
    __FREE(data);
    return 0;

}

//下发数据到子设备发送函数
int hm_48_app_to_subdev_frame_send(hm_48_valid_data_t *tvalid_data)
{
    char *data = NULL;
    data = hfmem_malloc(3 + tvalid_data->attrs_lenth + tvalid_data->tid.len);
    data[0] = PAYLOAD_DATA_TYPE_HM_APP_TO_SUBDEV;
    data[1] = shm_get_48_frame_seq();
    data[2] = tvalid_data->cmd_id;
    memcpy(data + 3, tvalid_data->tid.data, tvalid_data->tid.len);
    memcpy(data + 3 + tvalid_data->tid.len, tvalid_data->attrs_data, tvalid_data->attrs_lenth);
    shm_48_send_frame(data, 3 + tvalid_data->tid.len + tvalid_data->attrs_lenth);
    __FREE(data);
    return 0;
}