#include <hm_system.h>
#include <log.h>
#include <coroutine.h>
#include <glist.h>
#include <hm_module_devsend.h>
#include "ra_lan_comm.h"


#define UDP_SERVER_DEFAULT_PORT 10000
#define LAN_FRAM_TAIL	"\n"

static void udp_server_send_cb(void *arg);


typedef struct remote_params_s {
	struct
	{
		uint8_t *src;
		size_t len;
	}buf;

	remote_info_t remote;
}remote_params_t;

typedef struct MyStruct
{
	GList *send_data_queue;
	struct espconn handle;
	ETSTimer send_timer;
	ra_lan_comm_recv_callback_t recv_callback;
} lan_comm_t;

static lan_comm_t lan_comm;


 
void remote_param_destory(remote_params_t *p)
{
	if (p != NULL)
	{
		if (p->buf.src) free(p->buf.src);
		free(p);
	}
}

 
remote_params_t *remote_param_new(remote_info_t * remote, const void *src, size_t size)
{
	ASSERT(remote == NULL || src == NULL || size == 0);

	remote_params_t *new_prarm = (remote_params_t *)zalloc(sizeof(remote_params_t));
	ASSERT(new_prarm == NULL);

	new_prarm->buf.src = (uint8_t *)malloc(size);
	new_prarm->buf.len = size;
	ASSERT(new_prarm->buf.src == NULL);

	memcpy(new_prarm->buf.src, src, size);
	memcpy(&new_prarm->remote, remote, sizeof(remote_info_t));

	return new_prarm;
fail:
	remote_param_destory(new_prarm);
	return NULL;
}

 
static void send_data_format(remote_info_t *remote, char *str, size_t len)
{
	char *buf = NULL;
	uint32_t buf_len;

	/*pass '\0'*/
	size_t msg_len = len + sizeof(LAN_FRAM_TAIL);

	/*add '\n'*/
	buf_len = len + sizeof(LAN_FRAM_TAIL);
	buf = malloc(buf_len);
	ASSERT(buf == NULL);

	memcpy(buf, str, len);
	memcpy(buf + len, LAN_FRAM_TAIL, sizeof(LAN_FRAM_TAIL));

	/*send*/
	memcpy(lan_comm.handle.proto.udp->remote_ip, remote->remote_ip, 4);
	lan_comm.handle.proto.udp->remote_port = remote->remote_port;
	int ret = espconn_send(&lan_comm.handle, (uint8 *)buf, buf_len - 1);

	if unlikely(ret != 0)
	{
		ERR_MSG("[LAN] espconn_send error. %d", ret);
		goto fail;
	}
	else
	{
		//debug_print("%sbuf_len:%u, ip:%u.%u.%u.%u port:%d\n", buf, buf_len,
		//  remote->remote_ip[0],
		//	remote->remote_ip[1],
		//	remote->remote_ip[2],
		//	remote->remote_ip[3],
		//	remote->remote_port);		
		goto done;
	}


done:
fail:
	if (buf) { free(buf); }
	return;
}

/*
send one data packaget
*/
 
static void send_to_lan_queue_next(GList **p_data_list)
{
	if unlikely(*p_data_list == NULL)
	{
		goto none;
	}
	GList * current = *p_data_list;
	remote_params_t *data = (remote_params_t *)(*p_data_list)->data;
	send_data_format(&data->remote, (char *)data->buf.src, data->buf.len);
	remote_param_destory(data);
	*p_data_list = g_list_delete_link(*p_data_list, current);
	goto done;
done:
	return;
none:
	device_state_store(STATE_LAN_SEND_QUEUE_FINISH, 1);
	return;
}

  inline
static void list_data_clear_handler(gpointer data)
{
	remote_param_destory(data);
}

  inline
static void send_data_queue_clear(GList *list)
{
	g_list_free_full(list, &list_data_clear_handler);
}

 
static void udp_server_send_cb(void *arg)
{
	//device_state_store(STATE_LAN_SEND_QUEUE_FINISH, 0);
	//send_to_lan_queue_next(&lan_comm.send_data_queue);
}


 
static uint8_t __send_data_to_lan(remote_info_t *remote, void *src, size_t size)
{
	remote_params_t *data = NULL;

	send_data_format(remote, src, size);
	//ASSERT(lan_comm.handle.state == ESPCONN_NONE);
	/*
	if likely(device_state_load(STATE_LAN_SEND_QUEUE_FINISH) == 1)
	{
		device_state_store(STATE_LAN_SEND_QUEUE_FINISH, 0);
		send_data_format(remote, src, size);
	}
	else
	{
		//add send queue
	    ASSERT(system_get_free_heap_size() < SYSTEM_RESERVE_HEAP_SIZE);

		data = remote_param_new(remote, src, size);
		ASSERT(data == NULL);

		lan_comm.send_data_queue = g_list_append(lan_comm.send_data_queue, (gpointer)data);
		ASSERT(lan_comm.send_data_queue == NULL);

	}
	goto done;
done:
	return 1;
fail:
	if (lan_comm.send_data_queue != NULL) send_data_queue_clear(lan_comm.send_data_queue);
	lan_comm.send_data_queue = NULL;
	remote_param_destory(data);
	return 0;
	*/
}

 
int ra_lan_debug_hide(ra_uint8_t *data,const char *key,unsigned char len)
{
	char *ret = strstr(data,key);
	if (ret == NULL) return -1;
	if ((strstr(ret,":") - 1 - ret) == strlen(key))
	{
		ret = strstr(ret,":");
		if (ret == NULL) return -2;
		if ((*(ret + 2) == '"')||(*(ret + 1) != '"'))
			return 0;
		else
			memset(ret + 6,'*',len);
		return 0;
	}
	else 
		return -3;
	
}

 
int ra_lan_comm_send(ra_remote_info_t *remote, ra_uint8_t *data, ra_uint16_t size)
{
	ra_lan_debug_hide(data,"prodKey",24);
	ra_lan_debug_hide(data,"ctrlKey",24);
	ra_lan_debug_hide(data,"bindKey",24);
	ra_lan_debug_hide(data,"token",24);
	ra_lan_debug_hide(data,"license",24);
	
	__send_data_to_lan((remote_info_t *)remote, data, size);
	

}


  
static void cb_udp_server_recv(void *arg, char *pusrdata, unsigned short length)
{
	ra_remote_info_t remote;
	remot_info *remote_info = NULL;

	/*get remote ip*/
	espconn_get_connection_info(&lan_comm.handle, &remote_info, 0);
	memcpy(remote.remote_ip, remote_info->remote_ip, 4);
	remote.remote_port = remote_info->remote_port;
	

	//debug_print("\nLAN->len:%d, data = %sip:%u:%u:%u:%u port:%u\n", 
	//	length, pusrdata,
	//	remote.remote_ip[0],
	//	remote.remote_ip[1],
	//	remote.remote_ip[2],
	//	remote.remote_ip[3],
	//	remote.remote_port);

	if (lan_comm.recv_callback)
	{
		lan_comm.recv_callback(&remote, pusrdata, (ra_uint16_t)length);
		return;
	}
	else
	{
		port_info_t port_info;

		port_info.port = DATA_FROM_PORT_LAN;
		memcpy(port_info.remote.remote_ip, remote.remote_ip, 4);
		port_info.remote.remote_port = remote.remote_port;
    	jad_eval(&port_info, pusrdata, (uint16_t)length);
		debug_print("\nUDP_RecvFromLan->len:%d, data = %s\n", length, pusrdata);
		return;
	}
}

 
int ra_lan_comm_server_start(void)
{
	//if (&lan_comm.handle == NULL) 
	{
	static esp_udp udp;
	lan_comm.handle.type = ESPCONN_UDP;
	lan_comm.handle.proto.udp = &udp;
	lan_comm.handle.proto.udp->local_port = UDP_SERVER_DEFAULT_PORT;  // ESP8266 udp port

	int	ret = espconn_regist_recvcb(&lan_comm.handle, cb_udp_server_recv); // register a udp packet receiving callback
	if (ret != 0) { return -1; }

	ret = espconn_regist_sentcb(&lan_comm.handle, udp_server_send_cb);
	if (ret != 0)
	{
		ERR_MSG("\nregiter sentcb failed!!!\n");
		return;
	}

	ret = espconn_create(&lan_comm.handle);   // create udp
	if (ret != 0)
	{
		printf("[error]: UDP server start in port:%d ret:%d\n", UDP_SERVER_DEFAULT_PORT, ret);
		return RA_ERROR_INTERNAL;
	}

	device_state_store(STATE_LAN_SEND_QUEUE_FINISH, 1);
	device_state_store(STATE_LANCALL_AUTH_ENABLE, 1);
	return RA_SUCCESS;
	}
}

 
int ra_lan_comm_server_stop(void)
{
	int ret=espconn_regist_recvcb(&lan_comm.handle, NULL);
	if (ret != 0) { return RA_ERROR_INTERNAL; }
	ret = espconn_delete(&lan_comm.handle);
	if (ret != 0)
	{
		printf("[error]: UDP server start in port:%d ret:%d\n", UDP_SERVER_DEFAULT_PORT, ret);
		return RA_ERROR_INTERNAL;
	}
	return RA_SUCCESS;
}

 
void ra_lan_comm_register_recv_callback(ra_lan_comm_recv_callback_t callback)
{
	lan_comm.recv_callback = callback;
}