#include "hsf.h"
#include "hm_uart.h"
#include "hm_user_config.h"
#include "hm_uart_handle.h"
#include "hm_system.h"

HM_UART_EVENT_MEG_T ghm_uart_event_msg;
static HM_UART_MSG_TYPE shm_uart_check_frame(char *data, uint32_t len) ;
static bool shm_48_frame_sum_check(uart_frame_t *frame);
static int hm_uart_msg_send(char *data, uint32_t len);
hftimer_handle_t uart_timer;
uart_buffer_t uart_buffer;
int hm_uart_init(void)
{
    char rsp[64] = {0};
    hfat_send_cmd("AT+UART=9600,8,1,NONE,NFC,0\r\n", sizeof("AT+UART=9600,8,1,NONE,NFC,0\r\n"), rsp, 64);
    hfat_send_cmd("AT+NDBGL=0,0\r\n", sizeof("AT+NDBGL=0,0\r\n"), rsp, 64);
    hfat_send_cmd("AT+NDBGL=2,1\r\n", sizeof("AT+NDBGL=2,1\r\n"), rsp, 64);
    hfat_send_cmd("AT+CFGTF\r\n", sizeof("AT+CFGTF\r\n"), rsp, 64);
    memset(&ghm_uart_event_msg, 0, sizeof(HM_UART_EVENT_MEG_T));
    //启动串口处理任务
    process_start(&hm_uart_handle_process, NULL);
    uart_buffer.len = 0;
    memset(uart_buffer.data, 0, MAX_UART_BUFFER_LEN);
    hm_uart_timer_init();
    
    return 0;
}

int hm_uart_send_frame_to_device(char * data , uint32 len)
{
   char *pp = hex_to_str(data , len , false);
    u_printf("HM UART SEND DATA len = %d: \r\n", len);
    u_printf("[%s] \r\n", pp);
    __FREE(pp);
    return hfuart_send(HFUART0, data, len, 0);
}

void hm_uart_timeout_callback(hftimer_handle_t htimer)
{
    u_printf("HM UART time out callback\r\n");
    if (hftimer_get_timer_id(htimer) == UART_TIMER_ID)
    {
        hm_uart_msg_send(uart_buffer.data, uart_buffer.len );
        uart_buffer.len = 0;
        memset(uart_buffer.data, 0, MAX_UART_BUFFER_LEN);
        // hm_uart_timer_stop();
    }
}

void hm_uart_timer_init(void)
{
    if ((uart_timer = hftimer_create("UART-TIMER", UART_REC_TIMEOUT_TIME, false, UART_TIMER_ID, hm_uart_timeout_callback, 0)) == NULL)
    {
        u_printf("HM UART TIME create fail\n");
    }
}
void hm_uart_timer_start(void)
{
    hftimer_start(uart_timer);
}

void hm_uart_timer_stop(void)
{
    hftimer_stop(uart_timer);
}

static int hm_uart_msg_send(char *data, uint32_t len)
{
    char *pdata = NULL;
    HM_UART_MSG_TYPE tuart_msg_type = shm_uart_check_frame(data, len);
    if (tuart_msg_type == UART_INVALID_TYPE)
    {
        return -1;
    }
    else if (tuart_msg_type == UART_48_TYPE)
    {
        pdata = hfmem_malloc(len);
        memcpy(pdata, data, len);
        ghm_uart_event_msg.msg_type = UART_48_TYPE;
        ghm_uart_event_msg.msg_lenth = len;
        ghm_uart_event_msg.msg_data = pdata;
        process_post(&hm_uart_handle_process, PROCESS_EVENT_MSG, (void *)&ghm_uart_event_msg);
    }
    else if (tuart_msg_type == UART_AT_TYPE)
    {
        pdata = hfmem_malloc(len + 1);
        memcpy(pdata, data, len);
        pdata[len] = '\0';
        ghm_uart_event_msg.msg_type = UART_AT_TYPE;
        ghm_uart_event_msg.msg_lenth = len + 1;
        ghm_uart_event_msg.msg_data = pdata;
        process_post(&hm_uart_handle_process, PROCESS_EVENT_MSG, (void *)&ghm_uart_event_msg);
    }
    return 0;
}

static int hm_uart_get_data(char *data, uint32_t len)
{
    if((len + uart_buffer.len) > MAX_UART_BUFFER_LEN)
    {
        return -1;
    }
    else
    {
        memcpy(&(uart_buffer.data[uart_buffer.len]), data, len);
        uart_buffer.len = uart_buffer.len + len;
    }
    return 0;
    
}

/**
 * 串口接收回调函数
 * even :事件类型
 * data ：数据
 * len:本次接收到的数据长度
 * buf_len:设置的串口接收的缓冲区长度，固定值，例如：1024
 * */

int hm_uartRecv_callback(uint32_t event, char *data, uint32_t len, uint32_t buf_len)
{
    if (event == HFNET_UART0_DATA_READY)
    {
        u_printf("HM UART event is UART0 DATA RECEIVE \r\n");
        u_printf("HM UART len is %d \r\n", len);
        u_printf("HM UART data is  \r\n");
        //发送消息到串口处理任务
        //无效的串口数据
        hm_uart_get_data(data , len);
        hm_uart_timer_start();
    }
    return 0;
    
}

static HM_UART_MSG_TYPE shm_uart_check_frame(char *data, uint32_t len)
{
    uart_frame_t suart_48_frame = {data, len};
    if (len < 2)
    {
        return UART_INVALID_TYPE;
    }
    if (suart_48_frame.data[0] == UART_48_FRAME_HEAD_NUM)
    {
        if (shm_48_frame_sum_check(&suart_48_frame) == true)
        {
            return UART_48_TYPE;
        }
        else
            return UART_INVALID_TYPE;
    }
    if((data[0] == 'A' || data[0] == '+') && data[len-2]=='\r' && data[len-1] == '\n' )
    {
        return UART_AT_TYPE;
    }
    return UART_INVALID_TYPE;
}

static bool shm_48_frame_sum_check(uart_frame_t *frame)
{
    uint8_t sum = 0;
    uint8_t i;
    for (i = 0; i < frame->lenth - 1; i++)
    {
        sum += frame->data[i];
    }
    if (sum != frame->data[i])
    {
        return false;
    }
    return true;
}

uint8 hm_48_frame_sum_Calculate( char * data , uint32 len )
{
    uint8 sum = 0;
    uint8 i =0;
    for (i = 0; i < len; i++)
    {
        sum += data[i];
    }
    return sum;
}

