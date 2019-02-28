#include "hsf.h"
#include "cJSON.h"
#include "hm_parameter.h"
#include "hm_system.h"
#include "hm_connect_cloud_handle.h"
#include "hm_wifi_frame.h"
#include "hm_bind_json_protocol.h"
#include "hm_app_main.h"
#include "bin_version.h"

PROCESS(hm_connect_cloud_process, "hm_connect_cloud_process");

/*devLoginResp code*/
#define CLOUD_ERROR_LAST_TOKEN_NOT_USE 1400015
#define CLOUD_ERROR_TOKEN_NOT_VERIFY 1400017
#define DEVBIND_ERROR_PINCODE_OR_SSID 1400028
#define DEVBIND_ERROR_BIND_TIMEOUT 1400029
#define DEVBIND_ERROR_NOT_PUBLIC_APP 1400030
#define DEVBIND_ERROR_CANNOT_FORCE_BIND 1400031

#define JSON_PROTOCOL_ACTION_GET_PRODINFO "getProdInfo"
#define JSON_PROTOCOL_ACTION_GET_REQUESTVERIFY "requestVerify"
#define JSON_PROTOCOL_ACTION_GATEWAYBIND "gatewayBind"
#define JSON_PROTOCOL_ACTION_GATEWAYLOGIN "gatewayLogin"
#define JSON_PROTOCOL_ACTION_HEARTBEAT "heartbeat"
#define JSON_PROTOCOL_ACTION_REPORT_DEVINFO "reportDevInfo"

#define CC_MONITOR_LOOP_INTERVAL (30000u)                     //ms
#define CC_MONITOR_ERROR_TRIG_REBOOT_TIMEOUT (5 * 60 * 1000U) //(60*60*1000U)	//ms
#define RECV_TIMEOUT (10 * 1000U)                             //ms
#define CONN_TIMEOUT (5 * 1000U)                              //ms

#define HEARTBEAT_TIME 15 * 1000
#define HEARTBEAT_PROBE_TIME 3 * 1000
#define HEARTBEAT_MISS_THRESHOLD 3
#define CLOUD_FRAM_TAIL "\n"


#define ENCRYPT_TYPE_STR_NONE "None"

#define INFO_SERVICE_DOMAIN   "hub.aihomi.com"
#define INFO_SERVICE_PORT     91
hftimer_handle_t cloud_heartbeat_timer;
static char Hm_ServerAddr[64] = {0};
static int Hm_ServerPort = INFO_SERVICE_PORT;
volatile NETSOCKET socketNum = 0;
static NETSOCKET app_tcp_socket = -1;
static uint8 heartbeat_miss_times = 0;

static char print_buf[255] ={0};
enum TCP_CLIENT_FLAG
{
    TCP_CLIENT_WAIT = 0x0,
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
enum TCP_CLIENT_FLAG shm_tcp_connect_flag = TCP_CLIENT_WAIT;

void hm_set_socket(NETSOCKET socket)
{
    app_tcp_socket = socket;
}

NETSOCKET hm_get_socket(void)
{
    return app_tcp_socket;
}

static char tcp_send_buf[512];
int hm_tcp_send_data(char *data, unsigned short datalen)
{
    //申请内存
    char * pdata = hfmem_malloc(datalen + 1);
    memcpy(pdata , data , datalen);
    pdata[datalen] = '\n';

    u_printf("HM TCP SEND DATA : \r\n");
    if (datalen < 250)
    {
        u_printf("[%s] \r\n", pdata);
    }
    else if (datalen < 511)
    {
        memcpy(tcp_send_buf, pdata , 250);
        tcp_send_buf[250] = '\0';
        memcpy(&(tcp_send_buf[251]) , &(pdata[250]) , datalen-250);
        tcp_send_buf[1 + datalen] = '\0';
        u_printf("[%s", tcp_send_buf);
        u_printf("%s] \r\n", &(tcp_send_buf[251]));
    }  
    __FREE(data);
    return hm_tcp_frame_send(hm_get_socket(), pdata, datalen+1);
}

size_t cloud_creat_msgid(void)
{
    static size_t msgid = 0;
    return msgid++;
}

static void getProdInfo_report( void)
{
    cJSON *request = NULL;
    char *request_str = NULL;
    u_printf("HM TCP SEND getProdInfo_report\r\n");
    request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_GET_PRODINFO);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);
    
    char *tid = product_info_get_devtid();
    char *prodkey = product_info_get_prodkey();
    cJSON_AddStringToObject(params, "devTid", tid ? tid :"");
    cJSON_AddStringToObject(params, "prodKey", prodkey ? prodkey : "");

    request_str = cJSON_PrintUnformatted(request);
    cJSON_Delete(request);
    request = NULL;
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));
fail:
    return;
}

static void gatewaybind_report(void)
{
    char *request_str = NULL;
    u_printf("HM TCP SEND gatewaybind_report\r\n");
    cJSON *request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_GATEWAYBIND);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);

    char *tid = product_info_get_devtid();
    char *prodkey = product_info_get_prodkey();
    char *pin_code = product_info_get_pin_code();
    char *ssid = product_info_get_config_ssid();
    cJSON_AddStringToObject(params, "devTid", tid ? tid : "");
    cJSON_AddStringToObject(params, "prodKey", prodkey ? prodkey : "");
    cJSON_AddStringToObject(params, "PINCode", pin_code ? pin_code : "");
    cJSON_AddStringToObject(params, "SSID", ssid ? ssid : "");
    request_str = cJSON_PrintUnformatted(request);
    {
        cJSON_Delete(request);
        request = NULL;
    }
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));

fail:
    return ;
}


static void requestverify_report(void)
{
    char *request_str = NULL;
    u_printf("HM TCP SEND requestverify_report\r\n");
    cJSON *request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_GET_REQUESTVERIFY);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);

    char *tid = product_info_get_devtid();
    char *prodkey = product_info_get_prodkey();
    cJSON_AddStringToObject(params, "devTid", tid);
    cJSON_AddStringToObject(params, "prodKey", prodkey);

    request_str = cJSON_PrintUnformatted(request);
    {
        cJSON_Delete(request);
        request = NULL;
    }
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));

fail:
    return;
}

 void gatewaylogin_report(void)
{
    char *request_str = NULL;
    u_printf("HM TCP SEND gatewaylogin_report\n");
    cJSON *request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_GATEWAYLOGIN);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);

    char *tid = product_info_get_devtid();
    char *prodkey = product_info_get_prodkey();
    char *authkey = product_info_get_authkey();
    cJSON_AddStringToObject(params, "devTid", tid ? tid : "");
    cJSON_AddStringToObject(params, "prodKey", prodkey ? prodkey : "");
    cJSON_AddStringToObject(params, "authKey", authkey ? authkey : "");
    request_str = cJSON_PrintUnformatted(request);
    {
        cJSON_Delete(request);
        request = NULL;
    }
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));

fail:
    return;
}

 void reportdevinfo_report(void)
{
    u_printf("HM TCP SEND reportdevinfo_report\r\n");
    cJSON *request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, JSON_PROTOCOL_ACTION_REPORT_DEVINFO);

    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(request, JSON_PROTOCOL_FIELD_PARAMS, params);

    char *tid = product_info_get_devtid();
    char *mid = product_info_get_mid(); //don't free
    work_mode_t work_mode = product_info_get_work_mode();
    char *mac = get_mac_string(); //need free
    token_type_t token_type = product_info_get_token_type();
    char *bin_version = BIN_VERSION;
    char *service_host = product_info_get_service_hostname(); //don't free
    int32_t service_port = product_info_get_service_port();
    char *ssid = product_info_get_config_ssid();
    char *lan_ip = station_get_ip_string();

    cJSON_AddStringToObject(params, "devTid", tid);
    cJSON_AddStringToObject(params, "mid", mid ? mid : "");
    cJSON_AddNumberToObject(params, "workMode", work_mode);
    cJSON_AddStringToObject(params, "MAC", mac);
    cJSON_AddNumberToObject(params, "tokenType", token_type);
    cJSON_AddStringToObject(params, "binVer", bin_version ? bin_version : "");
    cJSON_AddStringToObject(params, "binType", "A");
    cJSON_AddStringToObject(params, "SDKVer", "2.1.0");
    cJSON_AddStringToObject(params, "serviceHost", service_host ? service_host : "");
    cJSON_AddNumberToObject(params, "servicePort", service_port);
    cJSON_AddStringToObject(params, "SSID", ssid);
    cJSON_AddStringToObject(params, "lanIp", lan_ip ? lan_ip : "");

    char *request_str = cJSON_PrintUnformatted(request);
    ASSERT(request_str == NULL);
    cJSON_Delete(request);
    hm_tcp_send_data(request_str, strlen(request_str));

    free(mac);
    free(lan_ip);
    return ;
fail:
    if (mac)
    {
        free(mac);
    }
    if (lan_ip)
    {
        free(lan_ip);
    }
    if (request)
    {
        cJSON_Delete(request);
    }
    return ;
}


void heartbeat_report(void)
{
    char *request_str = NULL;
    u_printf("HM TCP SEND heartbeat_report\n");
    cJSON *request = cJSON_CreateObject();
    cJSON_AddNumberToObject(request, JSON_PROTOCOL_FIELD_MSGID, cloud_creat_msgid());
    cJSON_AddStringToObject(request, JSON_PROTOCOL_FIELD_ACTION, "heartbeat");

    cJSON_AddNumberToObject(request, "rssi", 0);
    request_str = cJSON_PrintUnformatted(request);
    {
        cJSON_Delete(request);
        request = NULL;
    }
    ASSERT(request_str == NULL);
    hm_tcp_send_data(request_str, strlen(request_str));

fail:
    return;
}

void hm_clear_heartbeat_miss_times(void)
{
    heartbeat_miss_times = 0;
}
void hm_add_heartbeat_miss_times(void)
{
    heartbeat_miss_times ++;
}

uint8  hm_get_heartbeat_miss_times(void)
{
    return heartbeat_miss_times;
}

void hm_cloud_connect_heartbeat_callback(hftimer_handle_t htimer)
{
    u_printf("HM TCP SEND heartbeat_callback\r\n");
    if (hftimer_get_timer_id(htimer) == CLOUD_HEARTBEAT_TIMER_ID)
    {
        hm_add_heartbeat_miss_times();
        if (hm_get_heartbeat_miss_times() > HEARTBEAT_MISS_THRESHOLD)
        {
            //重新链接服务器
            hm_sys_run_info.run_info.cc_step_status = CC_START;
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
            return ;
        }
        else
        {
            heartbeat_report();
        }   
    }
}

 void hm_cloud_heartbeat_timer_init(void)
{
    if ((cloud_heartbeat_timer = hftimer_create("HEARTBEAT-TIMER", HEARTBEAT_TIME, true, CLOUD_HEARTBEAT_TIMER_ID, hm_cloud_connect_heartbeat_callback, 0)) == NULL)
    {
        u_printf("HM HEARTBEAT TIME create fail\n");
    }
}
void hm_cloud_heartbeat_timer_start(void)
{
    hftimer_start(cloud_heartbeat_timer);
}

void hm_cloud_heartbeat_timer_stop(void)
{
    hftimer_stop(cloud_heartbeat_timer);
}

static int hm_tcp_recv_callback(NETSOCKET socket, unsigned char *data, unsigned short len)
{
    data[len] = '\0';
    if (len >255)
    {
        memcpy(print_buf, data , 250);
        print_buf[250] = '\0';
        u_printf("HM TCP socket %d receive data with length %d\r\n", socket, len);
        u_printf("[%s", print_buf);
        u_printf("%s] \r\n", &(data[250]));
    }
    else
    {
        u_printf("HM TCP socket %d receive data with length %d\r\n", socket, len);
        u_printf("[%s] \r\n", data);
    }
    
        
    hm_cloud_json_handle(data,len);
   // process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
    return 0;
}

static void hm_tcp_connect_callback(NETSOCKET socket)
{
    u_printf("HM TCP socket %d connected.\r\n", socket);
    hm_free_all_tcp_frame();
    hm_set_socket(socket);
    shm_tcp_connect_flag = TCP_CLIENT_CONNECTED;
    //链接到设备信息服务器
    if (hm_sys_run_info.run_info.cc_step_status == CC_START)
    {
        //链接到设备信息服务器
        //开始进行
        hm_sys_run_info.run_info.cc_step_status = CC_CONNECTED_INFO_SERVER;
    }
    else
    {
        //链接到设备服务器
        /* code */
        hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_CONNECTED;
    }
    
    process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
}

static void hm_tcp_close_callback(NETSOCKET socket)
{

    u_printf("socket %d closed, current socket is %d.\r\n", socket, app_tcp_socket);
    if (socket == app_tcp_socket)
    {
        shm_tcp_connect_flag = TCP_CLIENT_CLOSING;
        if (hm_sys_run_info.run_info.cc_step_status != CC_RECEIVED_DEVICE_INFO)
        {
            hm_sys_run_info.run_info.cc_step_status = CC_START;
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
        }
        hm_free_all_tcp_frame();
    }
     
}

static void hm_tcp_send_callback(NETSOCKET socket)
{
    u_printf("TCP data send ack.\r\n");
    hm_free_one_tcp_frame();
   // process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
}

static NETSOCKET hm_tcp_client_connect_start(uip_ipaddr_t *addr, unsigned short port)
{
    struct tcp_socket t_socket;
    uip_ipaddr_copy(&t_socket.r_ip, addr);
    t_socket.r_port = port;
    t_socket.recv_callback = hm_tcp_recv_callback;
    t_socket.connect_callback = hm_tcp_connect_callback;
    t_socket.close_callback = hm_tcp_close_callback;
    t_socket.send_callback = hm_tcp_send_callback;
    t_socket.recv_data_maxlen = 1024;

    return hfnet_tcp_connect(&t_socket);
}

static int hm_connect_cloud_status_handle(void)
{
    switch (hm_sys_run_info.run_info.cc_step_status)
    {
        case CC_START:
        {
            u_printf("HM connect cloud CC_START\r\n");
            hm_cloud_heartbeat_timer_stop();
            strcpy(Hm_ServerAddr, INFO_SERVICE_DOMAIN);
            Hm_ServerPort = INFO_SERVICE_PORT;
            hm_sys_run_info.cloud_link_status = CLOUD_NOT_LINK;
            shm_tcp_connect_flag = TCP_CLIENT_WAIT;
            if (shm_tcp_connect_flag == TCP_CLIENT_CONNECTED)
            {
                hfnet_tcp_disconnect(app_tcp_socket); //关闭TCP链接
            }
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
            break;
        }
        case CC_GOT_INFO_SERVER_IP:
        {
            break;
        }
        case CC_CONNECTED_INFO_SERVER:
        {
            u_printf("HM connect cloud CC_CONNECTED_INFO_SERVER\r\n");
            hm_sys_run_info.cloud_link_status = CLOUD_LINK_HUB;
            getProdInfo_report();
            break;
        }

        case CC_RECEIVED_DEVICE_INFO:
        {
            char *thostname = product_info_get_service_hostname();
            u_printf("HM connect cloud CC_RECEIVED_DEVICE_INFO\r\n");
            strcpy(Hm_ServerAddr, thostname);
            Hm_ServerPort = product_info_get_service_port();
            u_printf("HM CC_RECEIVED_DEVICE_INFO  hostname is %s   port is  %d\r\n", Hm_ServerAddr, Hm_ServerPort);
            shm_tcp_connect_flag = TCP_CLIENT_WAIT;
            if (shm_tcp_connect_flag == TCP_CLIENT_CONNECTED)
            {
                hfnet_tcp_disconnect(app_tcp_socket); //关闭TCP链接
            }
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_MSG, NULL);
            break;
        }
        case CC_SPECIFIED_SERVER_GET_IP:
        {
            break;
        }
        case CC_SPECIFIED_SERVER_GOT_IP:
        {
            break;
        }
        case CC_SPECIFIED_SERVER_CONNECTED:
        {
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_CONNECTED\r\n");
            //网关登录开始
            requestverify_report();
            break;
        }
        case CC_SPECIFIED_SERVER_LOGIN_SUCCESS:
        {
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_LOGIN_SUCCESS\r\n");
            char * tpincode = product_info_get_pin_code();
            if (tpincode == NULL || strlen(tpincode) == 0)
            {
                hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_TOTAL_SUCCESS;
                process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
            }
            else
            {
                //绑定
                gatewaybind_report();
                product_info_clear_pincode(); //清除 pincode
            }
                break;
        }
        case CC_SPECIFIED_SERVER_LOGIN_FAIL:
        {
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_LOGIN_FAIL\r\n");
            hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_TOTAL_FAIL;
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
            break;
        }
        case CC_SPECIFIED_SERVER_BIND_SUCCESS:
        {
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_BIND_SUCCESS\r\n");
            hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_TOTAL_SUCCESS;
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
            break;
        }
        case CC_SPECIFIED_SERVER_BIND_FAIL:
        {
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_BIND_FAIL\r\n");
            hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_TOTAL_FAIL;
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
            break;
        }
        case CC_SPECIFIED_SERVER_TOTAL_SUCCESS:
        {
            //开始发送心跳
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_TOTAL_SUCCESS\r\n");
            hm_cloud_heartbeat_timer_start();
            product_info_clear_pincode();  //清除 pincode
            hm_sys_run_info.cloud_link_status = CLOUD_LINK_DEV;
            hm_sys_run_info.run_info.cc_step_status = CC_SPECIFIED_SERVER_NORMAL_DATA_TRANSTER;
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);          
            break ;     
        }
        case CC_SPECIFIED_SERVER_TOTAL_FAIL:
        {
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_TOTAL_FAIL\r\n");
            hm_sys_run_info.run_info.cc_step_status = CC_START;
            process_post(&hm_connect_cloud_process, PROCESS_EVENT_CONTINUE, NULL);
            break;
        }

        case CC_SPECIFIED_SERVER_NORMAL_DATA_TRANSTER :
        {
            u_printf("HM connect cloud CC_SPECIFIED_SERVER_NORMAL_DATA_TRANSTER\r\n");
            hm_sys_run_info.run_info.run_status = RUN_NORMAL_STATUS;
            process_post(&hm_app_main_process, PROCESS_EVENT_CONTINUE, NULL);          
            break ;
        }
        default:
            break;
    } 
   return 0;
}


PROCESS_THREAD(hm_connect_cloud_process, ev, data)
{
    PROCESS_BEGIN();
    static struct etimer timer_sleep;
    etimer_set(&timer_sleep, 3 * CLOCK_SECOND);
   
    while (1)
    {

        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_EXIT || ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_CONTINUE || ev == resolv_event_found);

        if(ev == PROCESS_EVENT_EXIT)
        {
            etimer_stop(&timer_sleep);
            hm_cloud_heartbeat_timer_stop();
            if(shm_tcp_connect_flag == TCP_CLIENT_CONNECTED)
            {
                hfnet_tcp_disconnect(app_tcp_socket); //关闭TCP链接
            }
            //退出链接任务
            hm_sys_run_info.cloud_link_status = CLOUD_NOT_LINK;
            shm_tcp_connect_flag = TCP_CLIENT_WAIT;
            u_printf("HM hm_connect_cloud_process  exit\r\n");
            break;
        }

        if (shm_tcp_connect_flag == TCP_CLIENT_WAIT)
        {
            if (check_link_state() == HM_LINK_NONE)
            {
                u_printf("HM wifi unconnected ");
                etimer_set(&timer_sleep, 1 * CLOCK_SECOND);
                continue;
            }
            else
            {
                u_printf("wifi connected.\r\n");
                shm_tcp_connect_flag = TCP_CLIENT_START;
            }
        }

        

        if (ev == PROCESS_EVENT_TIMER)
        {
            if ((shm_tcp_connect_flag == TCP_CLIENT_DNSING) || (shm_tcp_connect_flag == TCP_CLIENT_CONNECTING))
            {
                u_printf("DNS or connect tiemout.\r\n");
                hfnet_tcp_disconnect(app_tcp_socket);
                app_tcp_socket = -1;
                shm_tcp_connect_flag = TCP_CLIENT_START;
            }

            if ((shm_tcp_connect_flag == TCP_CLIENT_SENDING) || (shm_tcp_connect_flag == TCP_CLIENT_RECING))
            {
                u_printf("Send or recv tiemout.\r\n");
            }
        }

        if (shm_tcp_connect_flag == TCP_CLIENT_START)
        {
            uip_ipaddr_t addr;


            if (uiplib_ipaddrconv(Hm_ServerAddr, &addr) == 0)
            {
                //域名没有解析
                u_printf("DNS:%s.\r\n", Hm_ServerAddr);
                resolv_query(Hm_ServerAddr);
                shm_tcp_connect_flag = TCP_CLIENT_DNSING;
            }
            else
            {
                //域名解析成功
                app_tcp_socket = hm_tcp_client_connect_start(&addr, Hm_ServerPort);
                if (app_tcp_socket >= 0)
                    shm_tcp_connect_flag = TCP_CLIENT_CONNECTING;
                else
                    shm_tcp_connect_flag = TCP_CLIENT_START;
            }
            //10秒中的域名解析时间
            etimer_set(&timer_sleep, 10 * CLOCK_SECOND);
            continue;
        }
        else if ((shm_tcp_connect_flag == TCP_CLIENT_DNSING) && (ev == resolv_event_found))
        {
            //解析域名成功
            char *pHostName = (char *)data;
            if (strcmp(pHostName, Hm_ServerAddr))
                continue;

            uip_ipaddr_t addr;
            uip_ipaddr_t *addrptr = &addr;
            resolv_lookup(pHostName, &addrptr);
            u_printf("Resolv IP = %d.%d.%d.%d\n", addrptr->u8[0], addrptr->u8[1], addrptr->u8[2], addrptr->u8[3]);

            app_tcp_socket = hm_tcp_client_connect_start(addrptr, Hm_ServerPort);
            if (app_tcp_socket >= 0)
            {
                u_printf("HM DNS SUCCESS appsocket>0  = %d\n", app_tcp_socket);
                shm_tcp_connect_flag = TCP_CLIENT_CONNECTING;
            }
            else
            {
                u_printf("appsocket<0  = %d\n", app_tcp_socket);
                shm_tcp_connect_flag = TCP_CLIENT_START;
            }

            etimer_set(&timer_sleep, 10 * CLOCK_SECOND);
            continue;
        }
        else if (shm_tcp_connect_flag == TCP_CLIENT_CLOSING)
        {
            app_tcp_socket = -1;
            //判断是否重新去连接服务器
            shm_tcp_connect_flag = TCP_CLIENT_START;
        }
        else //connected/sendakc/recved/closed
        {
            etimer_set(&timer_sleep, 1 * CLOCK_SECOND);
        }

        if (ev == PROCESS_EVENT_CONTINUE)
        {
            hm_connect_cloud_status_handle();
            continue;
        }
    }

    PROCESS_END();
}



void hm_start_connect_cloud(void)
{
    strcpy(Hm_ServerAddr, INFO_SERVICE_DOMAIN );
    Hm_ServerPort = INFO_SERVICE_PORT;
    shm_tcp_connect_flag = TCP_CLIENT_WAIT;
    socketNum = 0;
    app_tcp_socket = -1;

    hm_sys_run_info.cloud_link_status = CLOUD_NOT_LINK;
    hm_sys_run_info.run_info.cc_step_status = CC_START;
    hm_cloud_heartbeat_timer_init();
    process_start(&hm_connect_cloud_process, NULL);
}
