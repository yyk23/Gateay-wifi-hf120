
#include "hsf.h"

static char *app_sendbuf = NULL;
static NETSOCKET app_udp_socket =-1;

static int app_udp_recv_callback(NETSOCKET socket, 
	unsigned char *data, 
	unsigned short len,
	uip_ipaddr_t *peeraddr, 
	unsigned short peerport)
{
	u_printf("UDP socket %d receive data with length %d, from %d.%d.%d.%d:%d\n", 
		socket, len, peeraddr->u8[0], peeraddr->u8[1], peeraddr->u8[2], peeraddr->u8[3], peerport);
	app_sendbuf=hfmem_malloc(len+16);
	data[len]='\0';
	sprintf(app_sendbuf, "Received: %s", data);
	
	hfnet_udp_sendto(socket, app_sendbuf, strlen(app_sendbuf), peeraddr, peerport);

	hfmem_free(app_sendbuf);
	return 0;
}

void udp_test_start(void)
{
	struct udp_socket u_socket;
	
	u_socket.l_port = 38899;
	u_socket.recv_callback = app_udp_recv_callback;
	u_socket.recv_data_maxlen = 1024;
	app_udp_socket = hfnet_udp_create(&u_socket);
	if(app_udp_socket<0)
	{
		HF_Debug(DEBUG_WARN,"create udp failed\n");
	}
}

