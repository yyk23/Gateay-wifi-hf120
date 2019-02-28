#ifndef __HM_UART_H__
#define __HM_UART_H_
#include "ra_types.h"
#include "c_types.h"
#include "hm_system.h"
#define   HM_UART_RATE_DEFAULT  "230400"

#define UART_48_FRAME_HEAD_NUM   0x48

#define UART_48_FRAME_HEAD_LENTH  0x04

#define UART_REC_TIMEOUT_TIME    20
#define MAX_UART_BUFFER_LEN      255
typedef struct
{
   uint8 * data ;
   uint8 lenth;
}uart_frame_t;

typedef struct
{
   char data[MAX_UART_BUFFER_LEN];
   uint32_t len;
}uart_buffer_t;



extern HM_UART_EVENT_MEG_T ghm_UART_event_msg;
extern int hm_uart_init(void) ;
extern int hm_uartRecv_callback(uint32_t event, char *data, uint32_t len, uint32_t buf_len);
extern int hm_uart_send_frame_to_device(char * ,uint32 );
extern uint8 hm_48_frame_sum_Calculate(char *data, uint32 len);

void hm_uart_timeout_callback(hftimer_handle_t htimer);
void hm_uart_timer_init(void);
void hm_uart_timer_start(void);
void hm_uart_timer_stop(void);

#endif