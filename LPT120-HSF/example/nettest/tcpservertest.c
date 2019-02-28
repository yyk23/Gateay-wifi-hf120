
#include "hsf.h"


PROCESS(server_test_process, "server_test_process");

#define CLIENT_MAX_CONNS 5

struct client_sock_data
{
	int flag;
	NETSOCKET sock_id;
	char *recv_buf;
	int recv_time;
};
static struct client_sock_data client_conn[CLIENT_MAX_CONNS];

static int find_sock_index(NETSOCKET socket)
{
	int i;
	
	for(i=0; i<CLIENT_MAX_CONNS; i++)
	{
		if((client_conn[i].flag != 0)&&(client_conn[i].sock_id == socket))
			return i;
	}

	return -1;
}

static int find_null_index(void)
{
	int i;
	
	for(i=0; i<CLIENT_MAX_CONNS; i++)
	{
		if(client_conn[i].flag == 0)
			return i;
	}

	return -1;
}

static int app_tcp_recv_callback(NETSOCKET socket, unsigned char *data, unsigned short len)
{
	u_printf("socket %d receive data with length %d\n", socket, len);
	int index = find_sock_index(socket);
	if(index < 0)
	{
		u_printf("not find socket from client list.\n");
		return 0;
	}

	if(client_conn[index].flag != 1)
	{
		u_printf("socket state not in recv ready.\n");
		return 0;
	}

	client_conn[index].flag = 2;
	if(client_conn[index].recv_buf == NULL)
	{
		client_conn[index].recv_buf = hfmem_malloc(len+16);
	}
	data[len]='\0';
	sprintf(client_conn[index].recv_buf, "Received: %s", data);
	client_conn[index].recv_time = hfsys_get_time();
	
	hfnet_tcp_send(socket, client_conn[index].recv_buf, strlen(client_conn[index].recv_buf));
	return 0;
}

static void app_tcp_close_callback(NETSOCKET socket)
{
	u_printf("socket %d closed\n", socket);
	int index = find_sock_index(socket);
	if(index < 0)
	{
		u_printf("not find socket from client list.\n");
		return;
	}

	client_conn[index].flag = 0;
	client_conn[index].sock_id = 0;
	if(client_conn[index].recv_buf != NULL)
	{
		hfmem_free(client_conn[index].recv_buf);
		client_conn[index].recv_buf = NULL;
	}
	client_conn[index].recv_time = 0;
}

static void app_accept_callback(NETSOCKET socket)
{
	u_printf("socket %d accept\n", socket);
	int index = find_null_index();
	if(index < 0)
	{
		u_printf("not find socket from client list.\n");
		hfnet_tcp_close(socket);
		return;
	}

	client_conn[index].flag = 1;
	client_conn[index].sock_id = socket;
	client_conn[index].recv_buf = NULL;
	client_conn[index].recv_time = hfsys_get_time();
}

static void app_tcp_send_callback(NETSOCKET socket)
{
	u_printf("TCP data send out by socket %d, free memory\n", socket);
	int index = find_sock_index(socket);
	if(index < 0)
	{
		u_printf("not find socket from client list.\n");
		return;
	}

	client_conn[index].flag = 1;
	if(client_conn[index].recv_buf != NULL)
	{
		hfmem_free(client_conn[index].recv_buf);
		client_conn[index].recv_buf = NULL;
	}
	client_conn[index].recv_time = hfsys_get_time();
}

PROCESS_THREAD(server_test_process, ev, data)
{
	PROCESS_BEGIN();
	static struct etimer timer_sleep;
	int i;
	
	etimer_set(&timer_sleep, 3 * CLOCK_SECOND);
	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
		
		for(i=0; i<CLIENT_MAX_CONNS; i++)
		{
			if(client_conn[i].flag != 0)
			{
				if(hfsys_get_time() - client_conn[i].recv_time > 30*1000)
				{
					u_printf("socket %d timeout, closed.[%d-%d]\n", client_conn[i].sock_id, hfsys_get_time() , client_conn[i].recv_time);
					
					hfnet_tcp_close(client_conn[i].sock_id);
					
					client_conn[i].flag = 0;
					client_conn[i].sock_id = 0;
					if(client_conn[i].recv_buf != NULL)
					{
						hfmem_free(client_conn[i].recv_buf);
						client_conn[i].recv_buf = NULL;
					}
					client_conn[i].recv_time = 0;
				}
			}
		}
		etimer_set(&timer_sleep, 3 * CLOCK_SECOND);
	}
	
	PROCESS_END();
}

void tcpserver_test_start(void)
{
	struct tcp_socket t_socket;
	
	t_socket.listen_port = 28899;
	t_socket.recv_callback = app_tcp_recv_callback;
	t_socket.close_callback = app_tcp_close_callback;
	t_socket.accept_callback = app_accept_callback;
	t_socket.send_callback = app_tcp_send_callback;
	t_socket.recv_data_maxlen = 1024;
	if(hfnet_tcp_listen(&t_socket) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start tcp server failed\n");
	}

	process_start(&server_test_process, NULL);
}

