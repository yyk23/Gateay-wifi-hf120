#include "hsf.h"
#include "cJSON.h"
#include "hm_link.h"
#include "hm_system.h"
#include "hm_app_main.h"

/*---------------------------------------------------------------------------*/
#define CLIENT_MAX_CONNS 5

PROCESS(hm_start_link_process, "hm start link");
PROCESS(hm_AP_mode_process, "hm AP mode Process");
struct client_sock_data
{
    int flag;
    NETSOCKET sock_id;
    char *recv_buf;
    int recv_time;
};
char ssid[64] = {0}, passwd[64] = {0}, pincode[7] = {0};

static struct client_sock_data client_conn[CLIENT_MAX_CONNS];

void hm_connect_wifi(char *ssidq, char *pswd)
{
    char send_buf[80] = {0};
    char rsp[64] = {0};
    char *words[6] = {NULL};
    u_printf("ssid: %s, passwd: %s\n", ssidq, pswd);
    sprintf(send_buf, "AT+WSSSID=%s\r\n", ssidq);
    hfat_send_cmd(send_buf, strlen(send_buf) + 1, rsp, 64);
    if (hfat_get_words(rsp, words, 6) > 0)
    {
        u_printf("\nAT+WSSSID=%s\r\n", words[1]);
    }
    memset(send_buf, 0, 80);
    sprintf(send_buf, "AT+WSKEY=WPA2PSK,AES,%s\r\n", pswd);
    hfat_send_cmd(send_buf, strlen(send_buf) + 1, rsp, 64);
    if (hfat_get_words(rsp, words, 6) > 0)
    {
        u_printf("\nAT+WSKEY=%s,%s,%s\r\n", words[1], words[2], words[3]);
    }
    
    hfat_send_cmd("AT+WMODE=STA\r\n", sizeof("AT+WMODE=STA\r\n"), rsp, 64);
}





static int find_sock_index(NETSOCKET socket)
{
    int i;

    for (i = 0; i < CLIENT_MAX_CONNS; i++)
    {
        if ((client_conn[i].flag != 0) && (client_conn[i].sock_id == socket))
            return i;
    }

    return -1;
}

static int find_null_index(void)
{
    int i;

    for (i = 0; i < CLIENT_MAX_CONNS; i++)
    {
        if (client_conn[i].flag == 0)
            return i;
    }

    return -1;
}

static void post_exit_ap_mode_process(void)
{
    process_post(&hm_AP_mode_process, PROCESS_EVENT_EXIT, NULL);
}

static int shm_ap_tcp_recv_callback(NETSOCKET socket, unsigned char *data, unsigned short len)
{

    u_printf("HM LINK socket %d receive data with length %d,data : %s\n", socket, len, data);
    static char goout = 0;
    if (goout == 0)
    {
        goout = 1;
        return -1;
    }
    //        unsigned char ssid[64] = {0}, passwd[64] = {0}, pincode[7] = {0};
    unsigned char *tmp = NULL, *passwd_tmp = NULL, *pincode_tmp = NULL;
    unsigned char ret = 0;
    tmp = "&password=";
    passwd_tmp = strstr(data, tmp);
    ret = passwd_tmp - data;
    strncpy(ssid, data + 5, ret - 5);
    u_printf("HM LINK ssid = %s, ret = %d\n", ssid, ret);

    tmp = "&pinCode=";
    pincode_tmp = strstr(passwd_tmp, tmp);
    ret = pincode_tmp - passwd_tmp;
    strncpy(passwd, passwd_tmp + 10, ret - 10);
    u_printf("passwd = %s, ret = %d\n", passwd, ret);

    strncpy(pincode, pincode_tmp + 9, 6);
    u_printf("pincode = %s, ret = %d\n", pincode, ret);

    if (goout == 1)
    {
        process_post_synch(&hm_start_link_process, PROCESS_EVENT_CONTINUE, NULL);
        goout++;
    }

    int index = find_sock_index(socket);
    if (index < 0)
    {
        u_printf("not find socket from client list.\n");
        return 0;
    }

    if (client_conn[index].flag != 1)
    {
        u_printf("socket state not in recv ready.\n");
        return 0;
    }

    client_conn[index].flag = 2;
    if (client_conn[index].recv_buf == NULL)
    {
        client_conn[index].recv_buf = hfmem_malloc(len + 16);
    }
    data[len] = '\0';
    sprintf(client_conn[index].recv_buf, "Received: %s", data);
    client_conn[index].recv_time = hfsys_get_time();

    return 0;
}

static void shm_ap_tcp_close_callback(NETSOCKET socket)
{
    u_printf("socket %d closed\n", socket);
    int index = find_sock_index(socket);
    if (index < 0)
    {
        u_printf("not find socket from client list.\n");
        return;
    }

    client_conn[index].flag = 0;
    client_conn[index].sock_id = 0;
    if (client_conn[index].recv_buf != NULL)
    {
        hfmem_free(client_conn[index].recv_buf);
        client_conn[index].recv_buf = NULL;
    }
    client_conn[index].recv_time = 0;
}

static void shm_ap_accept_callback(NETSOCKET socket)
{
    u_printf("socket %d accept\n", socket);
    int index = find_null_index();
    if (index < 0)
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

static void shm_ap_tcp_send_callback(NETSOCKET socket)
{
    u_printf("TCP data send out by socket %d, free memory\n", socket);
    int index = find_sock_index(socket);
    if (index < 0)
    {
        u_printf("not find socket from client list.\n");
        return;
    }

    client_conn[index].flag = 1;
    if (client_conn[index].recv_buf != NULL)
    {
        hfmem_free(client_conn[index].recv_buf);
        client_conn[index].recv_buf = NULL;
    }
    client_conn[index].recv_time = hfsys_get_time();
}

PROCESS_THREAD(hm_AP_mode_process, ev, data)
{
    PROCESS_BEGIN();
    static struct etimer timer_sleep;
    int i;

    etimer_set(&timer_sleep, 3 * CLOCK_SECOND);
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_EXIT || ev == PROCESS_EVENT_CONTINUE);
        if (ev == PROCESS_EVENT_EXIT)
        {
            break;
        }
        else if (ev == PROCESS_EVENT_CONTINUE)
        {
            continue;
        }
        for (i = 0; i < CLIENT_MAX_CONNS; i++)
        {

            if (client_conn[i].flag != 0)
            {
                if (hfsys_get_time() - client_conn[i].recv_time > 30 * 1000)
                {
                    u_printf("socket %d timeout, closed.[%d-%d]\n", client_conn[i].sock_id, hfsys_get_time(), client_conn[i].recv_time);

                    hfnet_tcp_close(client_conn[i].sock_id);

                    client_conn[i].flag = 0;
                    client_conn[i].sock_id = 0;
                    if (client_conn[i].recv_buf != NULL)
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

static int check_mode_state(void)
{
    char rsp[64] = {0};
    char *word[6] = {NULL};

    hfat_send_cmd("AT+WMODE\r\n", sizeof("AT+WMODE\r\n"), rsp, 64);
    if (hfat_get_words(rsp, word, 3) > 0)
    {
        u_printf(".............................word[1] = %s\n", word[1]);
        if (0 == strcmp(word[1], "AP"))
            return 0;
        else
            return 1;
    }
}

void hm_set_ap_mode(void)
{
    char rsp[64] = {0};
    char tsend_buf[64] ={0};
    char tmac[6]={0};
    hfat_send_cmd("AT+WMODE=AP\r\n", sizeof("AT+WMODE=AP\r\n"), rsp, 64);
    hfat_send_cmd("AT+LANN=192.168.4.1,255.255.255.0\r\n", sizeof("AT+LANN=192.168.4.1,255.255.255.0\r\n"), rsp, 64);
    hm_wifi_get_macaddr(tmac);
    sprintf(tsend_buf, "AT+WAP=11BGN,SmartDevice-%02X%02X,AUTO\r\n", tmac[4],tmac[5]);
    hfat_send_cmd(tsend_buf, sizeof("AT+WAP=11BGN,SmartDevice-0000,AUTO\r\n"), rsp, 64);
    hfat_send_cmd("AT+WAKEY=OPEN,NONE,\r\n", sizeof("AT+WAKEY=OPEN,NONE,\r\n"), rsp, 64);
    u_printf("HM set AP mode \n");
    hfat_send_cmd("AT+Z\r\n", sizeof("AT+Z\r\n"), rsp, 64);
}

static void hm_set_sniffer_AP_mode(void)
{
    struct tcp_socket t_socket;
    
    t_socket.listen_port = 7001;
    t_socket.recv_callback = shm_ap_tcp_recv_callback;
    t_socket.close_callback = shm_ap_tcp_close_callback;
    t_socket.accept_callback = shm_ap_accept_callback;
    t_socket.send_callback = shm_ap_tcp_send_callback;
    t_socket.recv_data_maxlen = 1024;

    if (hfnet_tcp_listen(&t_socket) != HF_SUCCESS)
    {
        u_printf("faile  liseten\n");
    }
    else
    {
        u_printf("dsadsadas,port : %d\n", t_socket.listen_port);
    }

    process_start(&hm_AP_mode_process, NULL);
}

static ATTRIBUTE_SECTION_KEEP_IN_SRAM void sniffer_recv_callback(uint8_t *pkt, uint32_t pkt_len)
{
    int i = 0;
    static int x = 4;
    static int j = 0, cur_index = 0, flag = 17;
    static char len = 0, get_ssid = 0, get_passwd = 0;
//        u_printf("\n  ..%d.. \n", pkt_len);
#if 0 
	for(i = 0; i < pkt_len; i++)
		u_printf("%02x ",pkt[i]);
#else
    for (i = x; i < (flag); i++)
    { //u_printf("           i = %d,  data = %02x\n",i,pkt[i]);
        if (pkt[0] == 0x88 && flag < 6)
        {
            //			u_printf("pkt0 == 88\n");
            break;
        }
        if ((pkt[i] == 0x01) && (pkt[i + 1] == 0x00) && (pkt[i + 2] == 0x5e))
        {

            if (pkt[i + 5] != 0xff)
            {
                //				u_printf("pkt 5   = %02x,%02x\n",pkt[i+4],pkt[i+5]);
                break;
            }
            if (len > 0)
            {
                if (get_ssid == 0 && pkt[i + 3] == cur_index)
                {
                    if (pkt[i + 4] == 0x0a)
                    {
                        ssid[j] = 0;
                        j = 0;
                        get_ssid = 1;
                        cur_index++;
                        u_printf("                                 sss\n");
                    }
                    else
                    {
                        ssid[j] = pkt[i + 4];
                        j++;
                        cur_index++;
                        u_printf("ssid = %s...%x\n", ssid, ssid[j - 1]);
                    }
                }
                else if (get_passwd == 0 && pkt[i + 3] == cur_index)
                {
                    if (pkt[i + 4] == 0x0a)
                    {
                        passwd[j] = 0;
                        j = 0;
                        get_passwd = 1;
                        cur_index++;
                        u_printf("                                 ppp\n");
                    }
                    else
                    {
                        passwd[j] = pkt[i + 4];
                        j++;
                        cur_index++;
                        u_printf("passwd = %s...%x\n", passwd, passwd[j - 1]);
                    }
                }
                else if ((get_ssid & get_passwd) == 1 && pkt[i + 3] == cur_index)
                {
                    if (len >= (cur_index + 1))
                    {
                        pincode[j] = pkt[i + 4];
                        j++;
                        u_printf("pincod = %s...%x\n", pincode, pincode[j - 1]);
                        u_printf("                                 bbb,index = %d,len=%d\n", cur_index, len);
                        if (len == (cur_index + 1))
                        {
                            process_post(&hm_start_link_process, PROCESS_EVENT_CONTINUE, NULL);
                            pincode[j] = 0;
                            j = 0;
                            len = 0;
                            flag = 17;
                            cur_index = 0;
                            get_ssid = 0;
                            get_passwd = 0;
                            u_printf("                                 aaa\n");
                            hfsniffer_stop();
                        }
                        else
                        {
                            u_printf("index = %x\n", cur_index);
                            cur_index++;
                        }
                    }
                }
                else
                {
                    //		u_printf("other  %d %s  %d  %x\n",cur_index,passwd,get_passwd,pkt[i+3]);
                }
            }
            else
            {
                if (pkt[i + 3] != 0x7f)
                {
                    //		u_printf("budeng  0x7f     %02x %02x %02x %02x %02x %02x\n",pkt[i],pkt[i+1],pkt[i+2],pkt[i+3],pkt[i+4],pkt[i+5]);
                    return;
                }
                else
                {
                    len = pkt[i + 4];
                    if (i < 10)
                    {
                        u_printf("i < 10");
                        flag = 5;
                        x = 4;
                    }
                    else
                    {
                        u_printf("i > 10");
                        x = 16;
                        flag = 17;
                    }
                    u_printf("len = %d\n", len);
                }
            }
            return;
        }
        else
        {
            //u_printf("isn't here\n");
            continue;
        }
    }
#endif
    return;
}

HM_LINK_STATUS check_link_state(void)
{
    char rsp[64] = {0};
    char *word[3] = {0};

    hfat_send_cmd("AT+WSLK\r\n", strlen("AT+WSLK\r\n"), rsp, 64);
    if (hfat_get_words(rsp, word, 3) > 0)
    {
        if (0 == strcmp(word[1], "Disconnected"))
            return HM_LINK_NONE;
        else
            return HM_LINK_OK;
    }
    return HM_LINK_NONE;
}

PROCESS_THREAD(hm_start_link_process, ev, data)
{
    static struct etimer periodic_timer;
    static struct etimer shm_sta_sniffer_timer;
    static uint8_t ch = 1;
    char rsp[64] = {0};
    char AP_mode_conn = 0;
    HM_LINK_MODE mode = HM_LINK_START_MODE_DEFAULT;
    PROCESS_BEGIN();

    etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 5000);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        u_printf("test sniffer start!\n");
        if (mode == HM_LINK_STA_MODE)
        {
            etimer_set(&shm_sta_sniffer_timer, CLOCK_SECOND * HM_LINK_TIMEOUT_TIMES);
            hfsniffer_start(HFSNIFFER_RECE_MODE_BROADCAST_DATA, sniffer_recv_callback);
            hfsniffer_set_filter(HFSNIFFER_RECE_MODE_BROADCAST_DATA);
            while (true)
            {
                etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 200);
                PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer) || ev == PROCESS_EVENT_CONTINUE);
                if (ev == PROCESS_EVENT_CONTINUE)
                {
                    mode = HM_LINK_SUCCESS;
                    process_post(&hm_start_link_process, PROCESS_EVENT_CONTINUE, NULL);
                    break;
                }
                if (etimer_expired(&shm_sta_sniffer_timer))
                {
                    mode = HM_LINK_AP_MODE;
                    break;
                }
                hfsniffer_set_channel(ch++);
                u_printf("HM LINK the channel = %d\n", ch);
                if (ch >= 14)
                    ch = 1;
            }
        }
        if (mode == HM_LINK_AP_MODE)
        {
            u_printf("HM start AP link \n");
            u_printf(" ap:%d..mode:%d\n", AP_mode_conn, mode);
            hm_set_sniffer_AP_mode();
            process_post_synch(&hm_AP_mode_process, PROCESS_EVENT_CONTINUE, NULL);
            etimer_set(&shm_sta_sniffer_timer, CLOCK_SECOND * HM_LINK_TIMEOUT_TIMES);
            while(true)
            {
                etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 200);
                PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer) || ev == PROCESS_EVENT_CONTINUE || etimer_expired(&shm_sta_sniffer_timer));
                if (ev == PROCESS_EVENT_CONTINUE)
                {
                    
                    mode = HM_LINK_SUCCESS;
                    break;
                }
                if (etimer_expired(&shm_sta_sniffer_timer))
                {
                    
                    mode = HM_LINK_FAIL;
                    break;
                }
            }
            
        }
        if (mode == HM_LINK_SUCCESS)
        {
            u_printf("HM  AP link success \n");
            product_info_set_pin_code(pincode);
            product_info_set_config_password(passwd);
            product_info_set_config_ssid(ssid);
            hm_connect_wifi(ssid, passwd);
            u_printf("pincode:%s,ssid:%s password: %s\n", pincode, ssid , passwd);   
            //重启模块  
            hfat_send_cmd("AT+Z\r\n", sizeof("AT+Z\r\n"), rsp, 64);
        }
        if(mode == HM_LINK_FAIL)
        {
            u_printf("HM  AP link timeout fail \n");
            hfat_send_cmd("AT+WMODE=STA\r\n", sizeof("AT+WMODE=STA\r\n"), rsp, 64);
            hfat_send_cmd("AT+Z\r\n", sizeof("AT+Z\r\n"), rsp, 64);
        }

    PROCESS_END();
}


int  ghm_start_link(void)
{
    process_start(&hm_start_link_process, NULL);
    u_printf("HM start wifi link\n");
    return 1;
}
