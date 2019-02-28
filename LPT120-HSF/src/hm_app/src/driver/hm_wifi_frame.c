#include "hsf.h"
#include "hm_user_config.h"
#include "hm_system.h"

#define MAX_TCP_FRAME_LENTH   20
char * tcp_frame_buf[MAX_TCP_FRAME_LENTH+1] ={NULL};
int  current_tcp_frame_num = 0;
static int  add_tcp_frame(void)
{
    if (current_tcp_frame_num >= MAX_TCP_FRAME_LENTH)
    {
        current_tcp_frame_num = MAX_TCP_FRAME_LENTH;
        return -1;
    }
    else
    {
        return current_tcp_frame_num++;
    }

}


int hm_tcp_frame_send(NETSOCKET socket_id, char *data, unsigned short datalen)
{

    if (add_tcp_frame() < 0)
    {
        __FREE(data);
        return -1;
    }
    tcp_frame_buf[current_tcp_frame_num - 1] = data;
    int re = hfnet_tcp_send(socket_id, data , datalen);  
    return re;
}

int hm_free_one_tcp_frame(void)
{
    int i =0;
    if (current_tcp_frame_num > 0)
    {
        __FREE(tcp_frame_buf[0]);
    }
    current_tcp_frame_num --;
    if (current_tcp_frame_num <= 0)
    {
        current_tcp_frame_num = 0;
    }
    else
    {
        for (i = 0; i < current_tcp_frame_num;i++)
        {
            tcp_frame_buf[i] = tcp_frame_buf[i+1];
        }
    }
    return 0;
    
}

int hm_free_all_tcp_frame(void)
{
    int i =0;
    for (i = 0; i < current_tcp_frame_num ;i++)
    {
        __FREE(tcp_frame_buf[i]);
    }
    memset(tcp_frame_buf, NULL, sizeof(tcp_frame_buf));
    current_tcp_frame_num = 0;
    return 0;
}

