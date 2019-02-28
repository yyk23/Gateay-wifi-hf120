
#include <hsf.h>
#include <string.h>
#include <stdio.h>
#include "httpstest.h"
#include <unistd.h>
#include <process.h>
#include "ssl_api.h"


/********************************** Globals ***********************************/
/************************* google *********************************/
static unsigned char g_httpRequestHdr1[] = "GET / HTTP/1.1\r\n"
	"Host: www.google.com.tw\r\n"
	"User-Agent: MatrixSSL/3.7.2-OPEN\r\n"
	"Accept: */*\r\n"
	"Content-Length: 0\r\n"
	"\r\n";

/************************* icomm-server *********************************/
static unsigned char g_httpRequestHdr2[] = "GET / HTTP/1.1\r\n"
	"Host: 192.168.55.58\r\n"
	"User-Agent: MatrixSSL/3.7.2-OPEN\r\n"
	"Accept: */*\r\n"
	"Content-Length: 0\r\n"
	"\r\n";

/************************* aliyun *********************************/
static unsigned char g_httpRequestHdr3[] = "GET /checkpreload.htm HTTP/1.1\r\n"
	"Host: iot.aliyun.com\r\n"
	"User-Agent: MatrixSSL/3.7.2-OPEN\r\n"
	"Accept: */*\r\n"
	"Content-Length: 0\r\n"
	"\r\n";

static unsigned short g_port;
static uip_ipaddr_t g_ipbyte1, g_ipbyte2, g_ipbyte3, g_ipbyte4,g_remote_ip;
static uint32 g_bytes_requested;
static unsigned char *g_httpRequestHdr=NULL ,*g_http_hostname=NULL;
ssl_t *gsslctx=NULL;
char httprsp[2048];

const char *sandbox_ca_cert =
"-----BEGIN CERTIFICATE-----\n"\
"MIIEoTCCA4mgAwIBAgIJAMQSGd3GuNGVMA0GCSqGSIb3DQEBBQUAMIGRMQswCQYD\n"\
"VQQGEwJDTjERMA8GA1UECBMIWmhlamlhbmcxETAPBgNVBAcTCEhhbmd6aG91MRYw\n"\
"FAYDVQQKEw1BbGliYWJhIEdyb3VwMQ4wDAYDVQQLEwVBbGluazEOMAwGA1UEAxMF\n"\
"QWxpbmsxJDAiBgkqhkiG9w0BCQEWFWFsaW5rQGFsaWJhYmEtaW5jLmNvbTAeFw0x\n"\
"NDA4MjkwMzA5NDhaFw0yNDA4MjYwMzA5NDhaMIGRMQswCQYDVQQGEwJDTjERMA8G\n"\
"A1UECBMIWmhlamlhbmcxETAPBgNVBAcTCEhhbmd6aG91MRYwFAYDVQQKEw1BbGli\n"\
"YWJhIEdyb3VwMQ4wDAYDVQQLEwVBbGluazEOMAwGA1UEAxMFQWxpbmsxJDAiBgkq\n"\
"hkiG9w0BCQEWFWFsaW5rQGFsaWJhYmEtaW5jLmNvbTCCASIwDQYJKoZIhvcNAQEB\n"\
"BQADggEPADCCAQoCggEBAMHr21qKVy3g1GKWdeGQj3by+lN7dMjGoPquLxiJHzEs\n"\
"6auxiAiWez8pFktlekIL7FwK5F7nH1px5W5G8s3cTSqRvunex/Cojw8LbNAStpXy\n"\
"HrqUuDhL3DYF7x87M/7H3lqFuIlucSKNC60Yc03yuIR1I/W0di40rDNeXYuCzXIv\n"\
"yheg+b7zD939HOe+RS878hDW5/p75FY+ChI8GA4dPEQb5fyRrjHAXneo+S8jdnqr\n"\
"SCjHSS7+jI36dyEfZ72rkLNJ3v1WboH02Rchj1fFIfagn+Ij4v0ruejOTIvc/ngD\n"\
"OLZNTUyF4B3EG4IAZRlO12jDECc4Com0yfFQ0IxkNVMCAwEAAaOB+TCB9jAdBgNV\n"\
"HQ4EFgQU9iyOWx+oGSOhdlpHeWMYsHXRwwkwgcYGA1UdIwSBvjCBu4AU9iyOWx+o\n"\
"GSOhdlpHeWMYsHXRwwmhgZekgZQwgZExCzAJBgNVBAYTAkNOMREwDwYDVQQIEwha\n"\
"aGVqaWFuZzERMA8GA1UEBxMISGFuZ3pob3UxFjAUBgNVBAoTDUFsaWJhYmEgR3Jv\n"\
"dXAxDjAMBgNVBAsTBUFsaW5rMQ4wDAYDVQQDEwVBbGluazEkMCIGCSqGSIb3DQEJ\n"\
"ARYVYWxpbmtAYWxpYmFiYS1pbmMuY29tggkAxBIZ3ca40ZUwDAYDVR0TBAUwAwEB\n"\
"/zANBgkqhkiG9w0BAQUFAAOCAQEAO7u7ozEES2TgTepq3ZTk1VD5qh2zhcSLLr+b\n"\
"yDQlkbm0lnah/GOGmpr/Wlr8JSXUJEWhsLcbnG1dhbP72DzFHri8ME4wO8hbeyXU\n"\
"7cFOHmP4DZi+Ia2gyy/GZ66P6L9df89MJzMOt46NYn+A3rI12M0qTJ6GNdUHz2R9\n"\
"VGkahs6bfGQGi0Se24jj4Es+MeAlrG5U0d0wVY5Dt4jpS9wHLupyAiANbj4Ls5x2\n"\
"6cwS4+Q4ErezFMnKiQ5VKc6S88ohYszt82CYMPEqIiZRkCfjsVz/r6vw2DFwN0Ox\n"\
"8Cb9myZwXypcOZbI7M+9W8909Z+TSHW1UlNkyrIsqDGuzE866w==\n"\
"-----END CERTIFICATE-----\n";


PROCESS(httpsClientConnection, "httpsClientConnection process");

PROCESS_THREAD(httpsClientConnection, ev, data)
{
	SSLMSG *msg;
    int rc;

    PROCESS_BEGIN();

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_MSG) || (ev == PROCESS_EVENT_EXIT) ||(resolv_event_found==ev));
		if( ev == PROCESS_EVENT_EXIT) 
		{
			break;
		}
		else if(ev == resolv_event_found)
		{
			uip_ipaddr_t addr;
			uip_ipaddr_t *addrptr = &addr;
			printf("hostname=%s %s\n",(char*)data,g_http_hostname);
			if(strcmp((char*)data,g_http_hostname)!=0)
				continue;
			
			resolv_lookup(g_http_hostname, &addrptr);
			uip_ipaddr_copy(&g_remote_ip, addrptr);
			printf("Resolv IP = %d.%d.%d.%d\n", g_remote_ip.u8[0], g_remote_ip.u8[1], g_remote_ip.u8[2], g_remote_ip.u8[3] );
			if(g_remote_ip.u16[0]==0&&g_remote_ip.u16[1]==0)
				continue;
			printf("SSL connect to server!\n");
			gsslctx = SSL_connect(&httpsClientConnection, g_remote_ip, g_port,g_http_hostname);
		}
		else if(ev == PROCESS_EVENT_MSG) 
        {
            msg = data;
            if(msg->status == SSL_CONNECTED)
            {
            	uint8_t test_data[]={0x57,0x53,0x46,0x31,0x00,0x00,0x00,0x11,0x14,0x00,0x00,0x00,0x03,0x00,0x00,0x01,0x0C };
                printf("SSL connected![%d]\n",hfsys_get_time());
                printf("SSl write data len:%d\n", strlen(g_httpRequestHdr));
                printf("%s\n", g_httpRequestHdr);
                SSL_write(gsslctx, test_data, sizeof(test_data));
            }
            else if(msg->status == SSL_CLOSED)
            {
                printf("SSL disconnect\n");
				if(gsslctx!=NULL)
				SSL_disconnect(gsslctx);
				gsslctx=NULL;
            }
            else if(msg->status == SSL_NEWDATA)
            {
                rc = SSL_read(gsslctx, httprsp, sizeof(httprsp));
                httprsp[rc] = 0;
                printf("recvlen:%d\n", rc);
                printf("rsp:%s\n", httprsp);
            }
            else if(msg->status == SSL_SENDACK)
            {
                printf("ssl sent complete\n");
            }
        }
    }

    PROCESS_END();
}

void https_send_get_test(void)
{
	if(gsslctx!=NULL)
	{
		uint8_t test_data[]={0x57,0x53,0x46,0x31,0x00,0x00,0x00,0x11,0x14,0x00,0x00,0x00,0x03,0x00,0x00,0x01,0x0C };
		SSL_write(gsslctx, test_data, sizeof(test_data));
		SSL_write(gsslctx, test_data, sizeof(test_data));
		SSL_write(gsslctx, test_data, sizeof(test_data));
	}
}

void httpstest(const char *hostname,const char *path,unsigned short port,char load_ca)
{
#if 1
	if(g_httpRequestHdr==NULL)
		g_httpRequestHdr = hfmem_malloc(strlen(hostname)+strlen(path)+128);

	if(g_http_hostname!=NULL)
	{
		hfmem_free(g_http_hostname);
		g_http_hostname=NULL;
	}
	g_http_hostname = hfmem_malloc(strlen(hostname)+1);
	strcpy(g_http_hostname,hostname);
	
	sprintf(g_httpRequestHdr,"GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: HF-MC300\r\nContent-Length: 0\r\nCache-Control: No-Cache\r\nConnection: Keep-Alive\r\nAccept: */*\r\n\r\n",path,hostname);
#else
    g_ipbyte1.u8[0] = 74;
    g_ipbyte1.u8[1] = 125;
    g_ipbyte1.u8[2] = 204;
    g_ipbyte1.u8[3] = 106;

    g_ipbyte2.u8[0] = 192;
    g_ipbyte2.u8[1] = 168;
    g_ipbyte2.u8[2] = 55;
    g_ipbyte2.u8[3] = 58;

    g_ipbyte3.u8[0] = 42;
    g_ipbyte3.u8[1] = 156;
    g_ipbyte3.u8[2] = 141;
    g_ipbyte3.u8[3] = 75;
    g_httpRequestHdr = g_httpRequestHdr3;	
#endif
	g_port               = port;

	if(gsslctx!=NULL)
		SSL_disconnect(gsslctx);
	
    process_start(&httpsClientConnection, NULL);
    SSL_init();
	//if(load_ca&&SSL_loadkeys(sandbox_ca_cert)!=0)
	//{
	//	printf("SSL_loadkeys fail\n");
	//}
	if(uiplib_ipaddrconv(hostname, &g_remote_ip)  == 0 )	
	{
		printf("DNS format : %s\n", hostname);
		resolv_query(hostname);
	}
	else
    	gsslctx = SSL_connect(&httpsClientConnection, g_remote_ip, g_port,g_http_hostname);
        
}
/////////////////////////////////////////////////////////////////////////////////
#define SSL_TEST_MODE_READ_WRITE 1
#define SSL_TEST_MODE_WRITE_ONLY 2
#define SSL_TEST_MODE_READ_ONLY 3
unsigned char ssltest_mode = SSL_TEST_MODE_READ_WRITE;
unsigned short ssltest_data_size = 0;
void SSL_Test_Delay()
{
	int i;
	
	for(i=0;i<10000;i++);
}

PROCESS(sslClientConnection, "sslClientConnection process");

PROCESS_THREAD(sslClientConnection, ev, data)
{
    SSLMSG *msg;
    int rc;

    PROCESS_BEGIN();

    while(1)
    {
        PROCESS_WAIT_EVENT_UNTIL((ev == PROCESS_EVENT_MSG) || (ev == PROCESS_EVENT_EXIT));
	if( ev == PROCESS_EVENT_EXIT) 
	{
		break;
	}
	else if(ev == PROCESS_EVENT_MSG) 
        {
            msg = data;
            if(msg->status == SSL_CONNECTED)
            {
		u_printf("SSL connected!\n");
		if(ssltest_mode == SSL_TEST_MODE_WRITE_ONLY)
		{
			SSL_write(gsslctx, httprsp, ssltest_data_size);	
		}
            }
            else if(msg->status == SSL_CLOSED)
            {
                u_printf("SSL disconnect\n");
		if(gsslctx!=NULL)
			SSL_disconnect(gsslctx);
		gsslctx=NULL;
            }
            else if(msg->status == SSL_NEWDATA)
            {
                rc = SSL_read(gsslctx, httprsp, sizeof(httprsp));
		if(ssltest_mode == SSL_TEST_MODE_READ_WRITE)
		{
		 	SSL_write(gsslctx, httprsp, rc/*sizeof(httprsp)*/);
			memset(httprsp,0xaa, rc);
			SSL_write(gsslctx, httprsp, rc/*sizeof(httprsp)*/);	
		}
            }
            else if(msg->status == SSL_SENDACK)
            {
		if(ssltest_mode == SSL_TEST_MODE_WRITE_ONLY)
		{
			SSL_write(gsslctx, httprsp, ssltest_data_size);	
		}
            }
        }
    }

    PROCESS_END();
}
void ssltest(const char *hostip,unsigned short port,unsigned short length,unsigned char mode)
{
	char rsp[4];
	
	if(uiplib_ipaddrconv(hostip, &g_remote_ip)  == 0)
	{
		return -1;
	}else{
		u_printf("SSL Server IP = %d.%d.%d.%d:%d\n", g_remote_ip.u8[0], g_remote_ip.u8[1], g_remote_ip.u8[2], g_remote_ip.u8[3], port);
	}

	g_port = port;
	ssltest_data_size = length;
	if(mode ==SSL_TEST_MODE_READ_WRITE || 
		mode ==SSL_TEST_MODE_WRITE_ONLY ||
		mode == SSL_TEST_MODE_READ_ONLY)
	{
		ssltest_mode = mode;
	}
		
	
	if(gsslctx!=NULL)
		SSL_disconnect(gsslctx);

	hfat_send_cmd("AT+DISPS=YES\r\n",sizeof("AT+DISPS=YES\r\n"),rsp,4);	
	
	process_start(&sslClientConnection, NULL);
	SSL_init();
    gsslctx = SSL_connect(&sslClientConnection, g_remote_ip, g_port,NULL);
	
}

