

/* hfnet.h
 *
 * Copyright (C) 2013-2020 ShangHai High-flying Electronics Technology Co.,Ltd.
 *				  	All rights reserved.
 *
 * This file is part of HSF.
 *
 *
 */

#ifndef _HF_NET_H_H_H_
#define _HF_NET_H_H_H_

#include "hsf.h"
#include "uip.h"

#define ASSIS_PORT 	48899
#define HTTP_API_BUF_SIZE	1400

typedef struct _hfnet_stat
{
	int socketa_send_bytes;
	int socketa_recv_bytes;
	int socketa_connect_times;
	int uart_send_bytes;
	int uart_recv_bytes;
	int socketb_send_bytes;
	int socketb_connect_times;
	int socketb_recv_bytes;
	int uart_buffer_full_times;
	int socketa_buffer_full_times;
	
}hfnet_stat;

extern hfnet_stat g_hfnet_stat;

typedef enum
{
	HFNET_SOCKETA_DATA_READY=0,
	HFNET_SOCKETA_CONNECTED=1,
	HFNET_SOCKETA_DISCONNECTED,
	HFNET_SOCKETB_DATA_READY,
	HFNET_SOCKETB_CONNECTED,
	HFNET_SOCKETB_DISCONNECTED,
	HFNET_UART0_DATA_READY,
	HFNET_UART1_DATA_READY,
	HFNET_ASSIS_DATA_READY
}hfnet_event_id_t;

typedef struct _hfnet_socketa_client
{
	int fd;
	uip_ip4addr_t addr;
}hfnet_socketa_client_t,*phfnet_socketa_client_t;

typedef int (*hfnet_callback_t)( uint32_t event,void *data,uint32_t len,uint32_t buf_len);

int  HSF_API hfnet_get_dhcp_ip (int * ip, int *mask, int *gw);

int HSF_IAPI hfnet_send_assis_event(uint32_t event,void *data,uint32_t len,uint32_t buf_len);

int HSF_API hfnet_start_assis_ex(uint16_t port,hfnet_callback_t p_callback);

int HSF_API hfnet_assis_write(char *rep,int len,uint32_t addr,uint16_t port);

int HSF_API hfnet_gethostbyname(const char *name, uip_ip4addr_t *addr);

int HSF_API hfnet_start_assis(uint16_t port);

int HSF_API hfnet_start_httpd(uint32_t uxpriority);

int HSF_API hfnet_start_socketb(uint32_t uxpriority,hfnet_callback_t p_uart_callback);

int HSF_API hfnet_start_socketa(uint32_t uxpriority,hfnet_callback_t p_uart_callback);

int HSF_API  hfnet_start_uart(uint32_t uxpriority,hfnet_callback_t p_uart_callback);

int HSF_API hfnet_start_uart_ex(uint32_t uxpriority,hfnet_callback_t p_callback, int stack_size);

int  HSF_API hfnet_socketa_send(char *data,uint32_t len,uint32_t timeouts);

int  HSF_API hfnet_socketa_eanble_clrtimeout_in_send(char b_enable);

int HSF_API hfnet_socketb_send(char *data,uint32_t len,uint32_t timeouts);

int HSF_IAPI hfnet_send_socketa_event(uint32_t event,void *data,uint32_t len,uint32_t buf_len);

int HSF_IAPI hfnet_send_socketb_event(uint32_t event,void *data,uint32_t len,uint32_t buf_len);

int HSF_API hfnet_socketa_fd(void);

int HSF_API hfnet_socketb_fd(void);

int HSF_API hfnet_socketa_get_client(int cid,phfnet_socketa_client_t p_client);

int HSF_IAPI hfnet_send_uart_event(uint32_t event,void *data,uint32_t len,uint32_t buf_len);

int HSF_IAPI hfnet_init(void);

int HSF_API hfnet_wifi_is_active(void);

int HSF_API  hfnet_wifi_up(void);

int HSF_API hfnet_wifi_down(void);

int HSF_API hfnet_set_udp_broadcast_port_valid(uint16_t start_port,uint16_t end_port);

/*socket API*/

typedef int (* tcp_socket_recv_callback_t)(NETSOCKET socket, 
					unsigned char *recv_data, 
					unsigned short recv_data_len);
typedef void (* tcp_socket_connect_callback_t)(NETSOCKET socket);
typedef void (* tcp_socket_close_callback_t)(NETSOCKET socket);
typedef void (* tcp_socket_accept_callback_t)(NETSOCKET socket);
typedef void (* tcp_socket_send_callback_t)(NETSOCKET socket);

struct tcp_socket {
  uip_ipaddr_t r_ip;

  unsigned short l_port;
  unsigned short r_port;
  unsigned short listen_port;
  
  tcp_socket_recv_callback_t recv_callback;
  tcp_socket_connect_callback_t connect_callback;
  tcp_socket_close_callback_t close_callback;
  tcp_socket_accept_callback_t accept_callback;
  tcp_socket_send_callback_t send_callback;

  unsigned char *recv_data;
  unsigned short recv_data_maxlen;
  unsigned short recv_data_len;

};

typedef int (* udp_socket_recv_callback_t)(NETSOCKET socket,
					unsigned char *recv_data, 
					unsigned short recv_data_len, 
					uip_ipaddr_t *peeraddr, 
					unsigned short peerport);
typedef void (* udp_socket_send_callback_t)(NETSOCKET socket);

struct udp_socket{
  uip_ipaddr_t r_ip;

  unsigned short l_port;
  unsigned short r_port;

  udp_socket_recv_callback_t recv_callback;
  //udp_socket_send_callback_t send_callback;
  
  unsigned char *recv_data;
  unsigned short recv_data_maxlen;
  unsigned short recv_data_len;
};

/** Protocol family and type of the netconn */
enum hfnet_netconn_type {
  NETCONN_INVALID    = 0,
  /* NETCONN_TCP Group */
  NETCONN_TCP        = 0x10,
  /* NETCONN_UDP Group */
  NETCONN_UDP        = 0x20,
  /* NETCONN_RAW Group */
  NETCONN_RAW        = 0x40
};

enum hfnet_netconn_state {
  NETCONN_STATE_NONE,
  NETCONN_STATE_WAIT,
  NETCONN_STATE_WRITE,
  NETCONN_STATE_READ,
  NETCONN_STATE_CREATE,
  NETCONN_STATE_LISTEN,
  NETCONN_STATE_CONNECT,
  NETCONN_STATE_CLOSE
};
struct hfnet_user_netconn{
	struct process *process;
	int type;
	int state;
	NETSOCKET driver_socket;
	union{
		struct tcp_socket tcp;
		struct udp_socket udp;
	}protocol;
};
int HSF_API hfnet_tcp_listen(struct tcp_socket *socket);
int HSF_API hfnet_tcp_unlisten(struct tcp_socket *socket);
int HSF_API hfnet_tcp_close(NETSOCKET socket_id);
NETSOCKET HSF_API hfnet_tcp_connect(struct tcp_socket *socket);
int HSF_API hfnet_tcp_disconnect(NETSOCKET socket_id);
int HSF_API hfnet_tcp_send(NETSOCKET socket_id, char *data, unsigned short datalen);
NETSOCKET HSF_API hfnet_udp_create(struct udp_socket *socket);
int HSF_API hfnet_udp_close(NETSOCKET socket_id);
int HSF_API hfnet_udp_sendto(NETSOCKET socket_id, char *data, unsigned short datalen,uip_ipaddr_t *peeraddr, unsigned short peerport);

#endif


