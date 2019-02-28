 
#include "hsf.h"

PROCESS(connect_test_process, "connect_test_process");

#define TEST_SERVER_ADDR "112.124.43.15"
#define TEST_SERVER_PORT 3006

enum TCP_CLIENT_FLAG
{
	TCP_CLIENT_WAIT=0x0,
	TCP_CLIENT_START,
	TCP_CLIENT_DNSING,
	TCP_CLIENT_DNSOK,
	TCP_CLIENT_CONNECTING,
	TCP_CLIENT_CONNECTED,
	TCP_CLIENT_SENDING,
	TCP_CLIENT_SENDACK,
	TCP_CLIENT_RECING,
	TCP_CLIENT_RECED,
	TCP_CLIENT_CLOSING,
	TCP_CLIENT_CLOSED
};

static NETSOCKET app_tcp_socket=-1;
static enum TCP_CLIENT_FLAG g_tcp_connect_flag = TCP_CLIENT_WAIT;

static int check_link_state(void)
{
	char rsp[64] = {0};
	char *word[3] = {0};

	hfat_send_cmd("AT+WSLK\r\n", strlen("AT+WSLK\r\n"), rsp, 64);
	if(hfat_get_words(rsp, word, 3)>0)
	{	
		if(0 == strcmp(word[1], "Disconnected"))
			return  0;
		else
			return 1;
	}

	return 0;
}

static void post_event_tcp_process(void)
{
	process_post(&connect_test_process, PROCESS_EVENT_CONTINUE, NULL);
}

static int app_tcp_recv_callback(NETSOCKET socket, unsigned char *data, unsigned short len)
{
	data[len] = 0;
	u_printf("socket %d receive data with length %d, [%s].\r\n", socket, len, data);

	post_event_tcp_process();
	return 0;
}

static void app_tcp_connect_callback(NETSOCKET socket)
{
	u_printf("socket %d connected.\r\n", socket);
	g_tcp_connect_flag = TCP_CLIENT_CONNECTED;

	post_event_tcp_process();
	
	hfnet_tcp_send(socket, "Hello", strlen("Hello"));
	u_printf("socket %d send data with length %d, [%s].\r\n", socket, strlen("Hello"), "Hello");
}

static void app_tcp_close_callback(NETSOCKET socket)
{
	u_printf("socket %d closed, current socket is %d.\r\n", socket, app_tcp_socket);
	if(socket == app_tcp_socket)
	{
		g_tcp_connect_flag = TCP_CLIENT_CLOSING;
	}

	post_event_tcp_process();
}

static void app_tcp_send_callback(NETSOCKET socket)
{
	u_printf("TCP data send ack.\r\n");
	
	post_event_tcp_process();
}

static NETSOCKET tcp_client_connect(uip_ipaddr_t *addr, unsigned short port)
{
	struct tcp_socket t_socket;
	
	uip_ipaddr_copy(&t_socket.r_ip, addr);
	t_socket.r_port = port;
	t_socket.recv_callback = app_tcp_recv_callback;
	t_socket.connect_callback = app_tcp_connect_callback;
	t_socket.close_callback = app_tcp_close_callback;
	t_socket.send_callback = app_tcp_send_callback;
	t_socket.recv_data_maxlen = 1024;

	return hfnet_tcp_connect(&t_socket);
}

PROCESS_THREAD(connect_test_process, ev, data)
{
	PROCESS_BEGIN();
	static struct etimer timer_sleep;

	etimer_set(&timer_sleep, 3 * CLOCK_SECOND);
	u_printf("wait wifi connect ...");
	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_CONTINUE || ev == resolv_event_found );
		if(g_tcp_connect_flag == TCP_CLIENT_WAIT)
		{
			if(check_link_state() == 0)
			{
				u_printf(".");
				etimer_set(&timer_sleep, 1 * CLOCK_SECOND);
				continue;
			}
			else
			{
				u_printf("wifi connected.\r\n");
				g_tcp_connect_flag = TCP_CLIENT_START;
			}
		}

		if(ev == PROCESS_EVENT_TIMER)
		{
			if((g_tcp_connect_flag == TCP_CLIENT_DNSING)||(g_tcp_connect_flag == TCP_CLIENT_CONNECTING))
			{
				u_printf("DNS or connect tiemout.\r\n");
				hfnet_tcp_disconnect(app_tcp_socket);
				app_tcp_socket = -1;
				
				g_tcp_connect_flag = TCP_CLIENT_START;
			}

			if((g_tcp_connect_flag == TCP_CLIENT_SENDING)||(g_tcp_connect_flag == TCP_CLIENT_RECING))
			{
				u_printf("Send or recv tiemout.\r\n");
			}
		}
	
		if(g_tcp_connect_flag == TCP_CLIENT_START)
		{
			uip_ipaddr_t addr;
			
			if(uiplib_ipaddrconv(TEST_SERVER_ADDR, &addr) == 0)
			{
				u_printf("DNS:%s.\r\n", TEST_SERVER_ADDR);
				resolv_query(TEST_SERVER_ADDR);
				g_tcp_connect_flag = TCP_CLIENT_DNSING;
			}
			else
			{
				app_tcp_socket = tcp_client_connect(&addr, TEST_SERVER_PORT);
				if(app_tcp_socket >= 0)
					g_tcp_connect_flag = TCP_CLIENT_CONNECTING;
				else
					g_tcp_connect_flag = TCP_CLIENT_START;
			}
			
			etimer_set(&timer_sleep, 10 * CLOCK_SECOND);
			continue;
		}
		else if((g_tcp_connect_flag == TCP_CLIENT_DNSING)&&(ev == resolv_event_found))
		{
			char *pHostName = (char*)data;
			if(strcmp(pHostName, TEST_SERVER_ADDR))
				continue;

			uip_ipaddr_t addr;
			uip_ipaddr_t *addrptr = &addr;
			resolv_lookup(pHostName, &addrptr);
			u_printf("Resolv IP = %d.%d.%d.%d\n", addrptr->u8[0], addrptr->u8[1], addrptr->u8[2], addrptr->u8[3] );

			app_tcp_socket = tcp_client_connect(addrptr, TEST_SERVER_PORT);
			if(app_tcp_socket >= 0)
				g_tcp_connect_flag = TCP_CLIENT_CONNECTING;
			else
				g_tcp_connect_flag = TCP_CLIENT_START;
	
			etimer_set(&timer_sleep, 10 * CLOCK_SECOND);
			continue;
		}
		else if(g_tcp_connect_flag == TCP_CLIENT_CLOSING)
		{
			app_tcp_socket = -1;
			g_tcp_connect_flag = TCP_CLIENT_START;
		}
		else//connected/sendakc/recved/closed
		{
			etimer_set(&timer_sleep, 1 * CLOCK_SECOND);
		}
	}
	
	PROCESS_END();
}

void tcpclient_test_start(void)
{
	process_start(&connect_test_process, NULL);
}

