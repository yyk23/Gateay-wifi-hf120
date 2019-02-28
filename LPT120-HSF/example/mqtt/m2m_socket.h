#ifndef __M2M_SOCKET_H__
#define __M2M_SOCKET_H__

#define MQTT_SOCKET_SEND_BUF_SIZE   (1024)
#define MQTT_SOCKET_RECV_BUF_SIZE   (1024)

#pragma pack(1)

typedef struct __socket_para__
{
	uint8_t recv_buf[MQTT_SOCKET_RECV_BUF_SIZE];
	int fd;
	uint8_t connect_status;
}socket_para;

typedef struct __topic_para__
{	
	uint16_t msg_id;
	char name[0];
}topic_para;


#pragma pack()
#endif