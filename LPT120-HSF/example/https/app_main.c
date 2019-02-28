#include "hsf.h"
#include "airkissv2.h"

#define DEVICE_TYPE 	"gh_ba0c25259dca"
#define DEVICE_ID		"accfxxxxxxxx"

char *app_sendbuf = NULL;
NETSOCKET app_udp_socket =-1, app_tcp_socket=-1;

/*---------------------------------------------------------------------------*/


int g_module_id = __HF_MODULE_ID__;

#if (__HF_MODULE_ID__==HFM_LPB120)

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HFM_NOPIN,	//HFGPIO_F_JTAG_TCK
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDO
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDI
	HFM_NOPIN,	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HFM_GPIO20,	//HFGPIO_F_UART0_TX
	HFM_NOPIN,	//HFGPIO_F_UART0_RTS
	HFM_GPIO19,	//HFGPIO_F_UART0_RX
	HFM_NOPIN,	//HFGPIO_F_UART0_CTS
	
	HFM_NOPIN,	//HFGPIO_F_SPI_MISO
	HFM_NOPIN,	//HFGPIO_F_SPI_CLK
	HFM_NOPIN,	//HFGPIO_F_SPI_CS
	HFM_NOPIN,	//HFGPIO_F_SPI_MOSI
	
	HFM_GPIO5,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_GPIO6,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HFM_GPIO15,	//HFGPIO_F_NLINK
	HFM_GPIO18,	//HFGPIO_F_NREADY
	HFM_GPIO2,	//HFGPIO_F_NRELOAD
	HFM_NOPIN,		//HFGPIO_F_SLEEP_RQ
	HFM_NOPIN,		//HFGPIO_F_SLEEP_ON
	
	HFM_NOPIN,		//HFGPIO_F_WPS
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};

#elif  (__HF_MODULE_ID__==HFM_LPT220)

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HFM_NOPIN,	//HFGPIO_F_JTAG_TCK
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDO
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDI
	HFM_NOPIN,	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HFM_GPIO20,	//HFGPIO_F_UART0_TX
	HFM_NOPIN,	//HFGPIO_F_UART0_RTS
	HFM_GPIO19,	//HFGPIO_F_UART0_RX
	HFM_NOPIN,	//HFGPIO_F_UART0_CTS
	
	HFM_NOPIN,	//HFGPIO_F_SPI_MISO
	HFM_NOPIN,	//HFGPIO_F_SPI_CLK
	HFM_NOPIN,	//HFGPIO_F_SPI_CS
	HFM_NOPIN,	//HFGPIO_F_SPI_MOSI
	
	HFM_GPIO5,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_GPIO6,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HFM_GPIO15,	//HFGPIO_F_NLINK
	HFM_GPIO3,	//HFGPIO_F_NREADY
	HFM_GPIO2,	//HFGPIO_F_NRELOAD
	HFM_NOPIN,		//HFGPIO_F_SLEEP_RQ
	HFM_NOPIN,		//HFGPIO_F_SLEEP_ON
	
	HFM_NOPIN,		//HFGPIO_F_WPS
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};


#elif (__HF_MODULE_ID__ == HFM_LPT120)

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HFM_NOPIN,	//HFGPIO_F_JTAG_TCK
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDO
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDI
	HFM_NOPIN,	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HFM_GPIO20,	//HFGPIO_F_UART0_TX
	HFM_NOPIN,	//HFGPIO_F_UART0_RTS
	HFM_GPIO19,	//HFGPIO_F_UART0_RX
	HFM_NOPIN,	//HFGPIO_F_UART0_CTS
	
	HFM_NOPIN,	//HFGPIO_F_SPI_MISO
	HFM_NOPIN,	//HFGPIO_F_SPI_CLK
	HFM_NOPIN,	//HFGPIO_F_SPI_CS
	HFM_NOPIN,	//HFGPIO_F_SPI_MOSI
	
	HFM_GPIO5,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_GPIO6,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HFM_GPIO15,	//HFGPIO_F_NLINK
	HFM_GPIO3,	//HFGPIO_F_NREADY
	HFM_GPIO2,	//HFGPIO_F_NRELOAD
	HFM_NOPIN,		//HFGPIO_F_SLEEP_RQ
	HFM_NOPIN,		//HFGPIO_F_SLEEP_ON
	
	HFM_NOPIN,		//HFGPIO_F_WPS
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};
#else
#error "Please define __HF_MODULE_ID__!!!"
#endif

static int user_at_cmd_httpstest(pat_session_t s,int argc,char *argv[],char *rsp,int len);
static int user_at_cmd_httpssend(pat_session_t s,int argc,char *argv[],char *rsp,int len);
static int user_at_cmd_ssltest(pat_session_t s,int argc,char *argv[],char *rsp,int len);


extern 	void https_send_get_test(void);

//https://alink.tcp.aliyun.com/checkpreload.htm
const hfat_cmd_t user_define_at_cmds_table[]=
{
	{"HTTPSTEST",user_at_cmd_httpstest,"   AT+HTTPSTEST: SSL Test.\r\n",NULL},
	{"HTTPSSEND",user_at_cmd_httpssend,"   AT+HTTPSSEND: SSL Send Test Data.\r\n",NULL},
	{"SSLTEST",user_at_cmd_ssltest,"   AT+SSLTEST: SSL throughput test.\r\n",NULL},
	{NULL,NULL,NULL,NULL}
};


static int user_at_cmd_httpssend(pat_session_t s,int argc,char *argv[],char *rsp,int len)
{
	https_send_get_test();
	return 0;
}

static int user_at_cmd_httpstest(pat_session_t s,int argc,char *argv[],char *rsp,int len)
{
	extern 	void httpstest(const char *hostname,const char *path,unsigned short port,char load_ca);

	if(argc==0)
	{
		//httpstest("wsf.smart.tbsandbox.com","/",80,1);
		httpstest("alink.tcp.aliyun.com","/",443,1);
		return 0;
	}
	if(argc<3)
		return -1;

	httpstest(argv[0],argv[1],atoi(argv[2]),0);
	return 0;
}

static int user_at_cmd_ssltest(pat_session_t s,int argc,char *argv[],char *rsp,int len)
{
	extern 	int ssltest(const char *hostip, unsigned short port, unsigned short length, unsigned char mode);
	int ret;
	
	if(argc<3)
		return -1;

	ret = ssltest(argv[0],atoi(argv[1]),atoi(argv[2]), atoi(argv[3]));
	return ret;
}

void app_tcp_recv_callback(NETSOCKET socket, unsigned char *data, unsigned short len)
{
	printf("socket %d receive data with length %d\n", socket, len);
	app_sendbuf=hfmem_malloc(len+16);
	data[len]='\0';
	sprintf(app_sendbuf, "Received: %s", data);
	
	hfnet_tcp_send(socket, app_sendbuf, strlen(app_sendbuf));
}

void app_tcp_connect_callback(NETSOCKET socket)
{
	char buf[]="Hello, world!";
	printf("socket %d connected\n", (int)socket);

	hfnet_tcp_send(app_tcp_socket, buf, strlen(buf));
}

void app_tcp_close_callback(NETSOCKET socket)
{
	printf("socket %d closed\n", (int)socket);
}

void app_accept_callback(NETSOCKET socket)
{
	printf("socket %d accept\n", (int)socket);
}

void app_tcp_send_callback(NETSOCKET socket)
{
	printf("TCP data send out by socket %d, free memory\n", (int)socket);
	hfmem_free(app_sendbuf);
	if(app_tcp_socket>=0)
		hfnet_tcp_disconnect(app_tcp_socket);
}

void app_udp_recv_callback(NETSOCKET socket, 
	unsigned char *data, 
	unsigned short len,
	uip_ipaddr_t *peeraddr, 
	unsigned short peerport)
{
	struct tcp_socket t_socket;

	printf("UDP socket %d receive data with length %d, from %d.%d.%d.%d:%d\n", 
		socket, len, peeraddr->u8[0], peeraddr->u8[1], peeraddr->u8[2], peeraddr->u8[3], peerport);
	app_sendbuf=hfmem_malloc(len+16);
	data[len]='\0';
	sprintf(app_sendbuf, "Received: %s", data);
	
	hfnet_udp_sendto(socket, app_sendbuf, strlen(app_sendbuf), peeraddr, peerport);

	hfmem_free(app_sendbuf);

	t_socket.l_port = 58899;
	t_socket.r_ip = *peeraddr;
	t_socket.r_port = 8899;
	t_socket.recv_callback = app_tcp_recv_callback;
	t_socket.connect_callback = app_tcp_connect_callback;
	t_socket.close_callback = app_tcp_close_callback;
	t_socket.send_callback = app_tcp_send_callback;
	t_socket.recv_data_maxlen = 256;

	app_tcp_socket = hfnet_tcp_connect(&t_socket);
	
}

int USER_FUNC  app_main(void)
{
	struct tcp_socket t_socket;
	struct udp_socket u_socket;

	
	u_printf("\n%s Start %s %s\n\n",g_hfm_name[__HF_MODULE_ID__],__DATE__,__TIME__);

	if(hfnet_start_assis(ASSIS_PORT) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start httpd fail\n");
	}

#ifdef SUPPORT_UART_THROUGH
	if(hfnet_start_socketa(0,NULL) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start socketa fail\n");
	}
	
	if(hfnet_start_socketb(1,NULL) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start socketb fail\n");
	}
	if(hfnet_start_uart_ex(0,NULL,4096) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
#else
	if(hfnet_start_uart_ex(0,NULL,1024) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
#endif

#if 0
	if(hfnet_start_airlink(DEVICE_TYPE,DEVICE_ID) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start airlink fail!\n");
	}
#endif

#if 0
	/*following are example of network socket usage*/
	t_socket.listen_port = 28899;
	t_socket.recv_callback = app_tcp_recv_callback;
	t_socket.close_callback = app_tcp_close_callback;
	t_socket.accept_callback = app_accept_callback;
	t_socket.send_callback = app_tcp_send_callback;
	t_socket.recv_data_maxlen = 256;
	if(hfnet_tcp_listen(&t_socket) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start tcp server failed\n");
	}

	u_socket.l_port = 38899;
	u_socket.recv_callback = app_udp_recv_callback;
	u_socket.recv_data_maxlen = 256;
	app_udp_socket = hfnet_udp_create(&u_socket);
	if(app_udp_socket<0)
	{
		HF_Debug(DEBUG_WARN,"create udp failed\n");
	}
#endif

	return 1;
}


