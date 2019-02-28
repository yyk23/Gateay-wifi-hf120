
#include "hsf.h"
#include "mqttlib.h"
#include "m2m_socket.h"
#include "socket_driver.h"


#define MQTT_SERVER_ADDR  "iot.eclipse.org"
#define MQTT_SERVER_PORT  1883

#define MQTT_KEEPALIVE_SEC  (3*60)

socket_para g_mqtt_socket_para;
static mqtt_broker_handle_t g_stMQTTBroker;

char sub_topic_name[] = "/HF_mc300/subscribe/demo";
char pub_topic_name1[] = "/HF_mc300/publish/Qos0";
char pub_topic_name2[] = "/HF_mc300/publish/Qos1";
char pub_topic_name3[] = "/HF_mc300/publish/Qos2";
char pub_msg1[] = "Publich msg example:Qos 0";
char pub_msg2[] = "Publich msg example:Qos 1";
char pub_msg3[] = "Publich msg example:Qos 2";
topic_para *g_sub_topic = NULL;
topic_para *g_pub_topic1 = NULL;
topic_para *g_pub_topic2 = NULL;
topic_para *g_pub_topic3 = NULL;



PROCESS(mqtt_socket_process, "mqtt_socket_process");

void mqtt_socket_start(void)
{
	process_start(&mqtt_socket_process, NULL);
}

void mqtt_socket_exit(void)
{
	process_exit(&mqtt_socket_process);
}


void mqtt_tcp_socket_init(void)
{
	g_mqtt_socket_para.fd = -1;
    g_mqtt_socket_para.connect_status = SOCKET_CLOSED;
}

void mqtt_tcp_connect(char *addr, int port)
{
	uip_ip4addr_t remote_ip_addr;
	
	if(uiplib_ipaddrconv(addr, &remote_ip_addr) == 0)
	{
		resolv_query(addr);
		return;
	}
	else
	{
		g_mqtt_socket_para.connect_status = SOCKET_CREATE;
		g_mqtt_socket_para.fd = tcpconnect(&remote_ip_addr, port, &mqtt_socket_process);

		printf("Tcp socket(%d) connectting to [%d.%d.%d.%d:%d]\n", g_mqtt_socket_para.fd, 
		remote_ip_addr.u8[0], remote_ip_addr.u8[1], remote_ip_addr.u8[2], remote_ip_addr.u8[3], port);
	}
		
}


void app_mqtt_init(mqtt_broker_handle_t *broker)
{
	mqtt_init(broker, "HF_mc300");
	mqtt_init_auth(broker, NULL, NULL);
	mqtt_set_alive(broker, MQTT_KEEPALIVE_SEC);
}

int mqtt_send_packet(void* socket_info, const void* buf, unsigned int count)
{
	NETSOCKET fd = *((NETSOCKET*)socket_info);
	int ret = 0;

	ret = tcpsend(fd, buf, count);
	if(ret < 0)
		printf("tcp send fail\n");

	return ret;
}


void mqtt_login_server(mqtt_broker_handle_t *broker)
{
	broker->socket_info = (void*)&(g_mqtt_socket_para.fd);
	broker->send = mqtt_send_packet;

	if(mqtt_connect(broker)== -1)
		printf("Mqtt login server fail!\n");
	else
		printf("Do mqtt login ...\n");
}

void init_topic(topic_para **topic, char *name, int name_len)
{
	*topic = (topic_para *)hfmem_malloc(sizeof(topic_para) + name_len*sizeof(char));
	if(*topic == NULL)
	{
		printf("NO memory!\n");
		return ;
	}

	memset(*topic, 0, sizeof(topic_para) + name_len*sizeof(char));
	(*topic)->msg_id = 0;
	memcpy((*topic)->name, name, name_len);
}

void deinit_topic(topic_para **topic)
{
	hfmem_free(*topic);
	*topic = NULL;
}

void app_parse_mqttmsg(uint8_t *packet_buffer)
{
	uint8_t msg_type = 0;
	uint16_t msg_id_rcv = 0;
	char topic_name[56]={0};
	char msg[128]={0};
			
	msg_type = MQTTParseMessageType(packet_buffer);
	//printf("-----> parse:0x%02X\n", msg_type);
	switch(msg_type)
	{
		case MQTT_MSG_CONNACK:
			if(packet_buffer[3] == 0)
			{
				printf("Mqtt login server success\n");

				/* subscribe  */
				init_topic(&g_sub_topic, sub_topic_name, sizeof(sub_topic_name));
				mqtt_subscribe(&g_stMQTTBroker, g_sub_topic->name, &(g_sub_topic->msg_id));

				/* publish msg with Qos 0 */
				//step1:>>>publish               
				init_topic(&g_pub_topic1, pub_topic_name1, sizeof(pub_topic_name1));
				mqtt_publish(&g_stMQTTBroker, g_pub_topic1->name, pub_msg1, 0);

				printf("APP publish msg[%s] with Qos 0\n", pub_msg1);
				deinit_topic(&g_pub_topic1);
				
				/* publish msg with Qos 1 */
				//step1:>>>publish
				//step2:<<<puback
				init_topic(&g_pub_topic2, pub_topic_name2, sizeof(pub_topic_name2));
				mqtt_publish_with_qos(&g_stMQTTBroker, g_pub_topic2->name, pub_msg2, 0, 1, &(g_pub_topic2->msg_id));

			}
			else
				printf("Mqtt login server fail!\n");
			break;

		case MQTT_MSG_SUBACK:
			msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
			if(g_sub_topic && msg_id_rcv == g_sub_topic->msg_id)
				printf("Subcribe topic[%s] success\n", g_sub_topic->name);
			break;

		case MQTT_MSG_PUBLISH:
			mqtt_parse_pub_topic(packet_buffer, topic_name);	
			mqtt_parse_publish_msg(packet_buffer, msg);
			printf("****** Topic[%s] recv msg: *****\n%s\n",topic_name, msg);

			/* unsubscribe */
			if(!strcmp(g_sub_topic->name, topic_name))
				mqtt_unsubscribe(&g_stMQTTBroker, g_sub_topic->name, &(g_sub_topic->msg_id));
			
			break;

		case MQTT_MSG_UNSUBACK:
			msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
			if(g_sub_topic && msg_id_rcv == g_sub_topic->msg_id)
			{
				printf("Unsubcribe topic[%s] success\n", g_sub_topic->name);
				deinit_topic(&g_sub_topic);
			}
			break;	

		case MQTT_MSG_PUBACK://Qos1
			msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
			if(g_pub_topic2 && msg_id_rcv == g_pub_topic2->msg_id)
			{
				printf("APP publish msg[%s] with Qos 1\n", pub_msg2);
				deinit_topic(&g_pub_topic2);

				/* publish msg with Qos 2 */
				//step1:>>>publish
				//step2:<<<pubrec
				//step3:>>>pubrel
				//step4:<<<pubcomp
				init_topic(&g_pub_topic3, pub_topic_name3, sizeof(pub_topic_name3));
				mqtt_publish_with_qos(&g_stMQTTBroker, g_pub_topic3->name, pub_msg3, 1, 2, &(g_pub_topic3->msg_id));
			}
			break;

		case MQTT_MSG_PUBREC://Qos2
			msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
			if(g_pub_topic3 && msg_id_rcv == g_pub_topic3->msg_id)
				mqtt_pubrel(&g_stMQTTBroker, g_pub_topic3->msg_id);
			break;

		case MQTT_MSG_PUBCOMP://Qos2
			msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
			if(g_pub_topic3 && msg_id_rcv == g_pub_topic3->msg_id)
			{
				printf("APP publish msg[%s] with Qos 2\n", pub_msg3);
				deinit_topic(&g_pub_topic3);
			}	
			break;

		default:
			printf("Unknow mqtt msg type\n");
			break;
	}
		
}


PROCESS_THREAD(mqtt_socket_process, ev, data)
{
	PROCESS_BEGIN();
	
    SOCKETMSG msg;
	static struct etimer mqtt_socket_timer;
	int recv;


	mqtt_tcp_socket_init();
	app_mqtt_init(&g_stMQTTBroker);
	mqtt_tcp_connect(MQTT_SERVER_ADDR, MQTT_SERVER_PORT);

	while(1) 
	{
		PROCESS_WAIT_EVENT();

		if(ev == resolv_event_found)
		{
			char *p_hostname = (char *)data;

			if(strcmp(p_hostname, MQTT_SERVER_ADDR))
				continue;
			
			uip_ipaddr_t addr;
			uip_ipaddr_t *p_addr = &addr;
			uip_ip4addr_t remote_ip_addr;
			resolv_lookup(p_hostname, &p_addr);
			uip_ipaddr_copy(&remote_ip_addr, p_addr);

			char ip_str[20]= {0}; 
			sprintf(ip_str,"%d.%d.%d.%d", remote_ip_addr.u8[0], remote_ip_addr.u8[1], remote_ip_addr.u8[2], remote_ip_addr.u8[3]);
		
			mqtt_tcp_connect(ip_str, MQTT_SERVER_PORT);	
		}
		else if(ev == PROCESS_EVENT_EXIT)
		{
			mqtt_disconnect(&g_stMQTTBroker);
			tcpclose(g_mqtt_socket_para.fd);
			break;
		}
		else if(ev == PROCESS_EVENT_MSG)
		{
            msg = *(SOCKETMSG *)data;

			if(msg.status == SOCKET_CONNECTED) 
			{
				printf("MQTT tcp socket(%d) connected\n", msg.socket);
				g_mqtt_socket_para.connect_status = SOCKET_CONNECTED;
				tcp_set_send_buffer_size(g_mqtt_socket_para.fd, MQTT_SOCKET_SEND_BUF_SIZE);

				mqtt_login_server(&g_stMQTTBroker);
			}
			else if(msg.status == SOCKET_CLOSED)
			{
				if(g_mqtt_socket_para.connect_status == SOCKET_CREATE)
					printf("Socked(%d) create fail and retry...\n", msg.socket);
				else
					printf("MQTT tcp socket(%d) closed\n", msg.socket);		

				g_mqtt_socket_para.fd = -1;
				g_mqtt_socket_para.connect_status = SOCKET_CLOSED;
				etimer_set(&mqtt_socket_timer, 3*CLOCK_SECOND);
				PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&mqtt_socket_timer));
				
				mqtt_tcp_connect(MQTT_SERVER_ADDR, MQTT_SERVER_PORT);				
			}
			else if(msg.status == SOCKET_SENDACK)
			{
			
			}
			else if(msg.status == SOCKET_NEWDATA)
			{
				g_mqtt_socket_para.connect_status = SOCKET_NEWDATA;

				recv = tcprecv(g_mqtt_socket_para.fd, g_mqtt_socket_para.recv_buf, MQTT_SOCKET_RECV_BUF_SIZE);
				if(recv<=0)
				{
					printf("tcprecv error! ret:%d\n", recv);
					continue;
				}
				
				app_parse_mqttmsg(g_mqtt_socket_para.recv_buf);	
			}
		
		}
		

	}
	
	PROCESS_END();
}








