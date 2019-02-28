#ifndef __HM_SYSTEM_H__
#define __HM_SYSTEM_H__

#include "c_types.h"
#include "hm_link.h"
#include "hm_connect_cloud_handle.h"
#define SDK_VERSION  "2.10"
#define		FLASH_SIZE		2048
#if (FLASH_SIZE==1024)
#define DEVNAME "ESP_1M_"
#elif (FLASH_SIZE==2048)
#define DEVNAME "ESP_2M_"
#endif
#define DEVTID_LENGTH_MAX sizeof(DEVNAME) + 12

#define	NO_ENCODE  		0
#define	ADD_ENCODE		1
#define	LENGTH_ENCODE	2
#define	SOFTAP_ENCODE	3
#define	AIRKISS_ENCODE	4

#define KEY_LENGTH_MAX		32

#define CHECK_NAME		    "check"

#define PRODKEY_NAME		"prodKey"
#define PRODKEY_LENGTH_MAX	32

#define DEVTID_NAME         "devTid"
#define DEVPRIKEY_NAME      "devPriKey"

#define TOKEN_NAME			"token"
#define TOKEN_LENGTH_MAX	32
#define AUTHKEY_NAME		"authKey"
#define AUTHORIZATION_LENGTH_MAX	32
#define CTRLKEY_NAME		"ctrlKey"
#define CTRLKEY_LENGTH_MAX	32
#define BINDKEY_NAME		"bindKey"
#define BINDKEY_LENGTH_MAX	32
#define SERVER_NAME		    "server"
#define SERVER_LENGTH_MAX	32
#define SERVER_PORT_NAME	"port"
#define UART_BAUDRATE		"uart_baud_rate"
#define MCU_VERSION_NAME    "mcuVersion"
#define WIFI_TEST_NAME	    "wifiTest"
#define FACTORY_STRING_TEST_NAME "factoryStringTest"
#define CONFIG_TYPE			"configType"
#define ENCRYPT_TYPE		"encryptType"
#define PINCODE_NAME        "pincode"

#define WIFI_SSID_NAME           "ssid"
#define WIFI_PASSWORD_NAME       "password"

#define AIRKISS_WECHAT_ID	"weChatId"
#define AIRKISS_WECHAT_PRODUCT_ID	"wechatProductId"
#define AIRKISS_PARAMS_LENGTH_MAX 32

#define WIFI_TEST_FLAG    0x55

#define WIFI_CONFIG_FLAG  0x66

#define USER_BIN1_ADDR	0x01000
#define USER_BIN2_ADDR	0x81000

#define		DEVTID_SIZE		sizeof(""DEVNAME"XXXXXXXXXXXX")

#define BIT(nr)                 (1UL << (nr))

#define REG_SET_BIT(_r, _b)  (*(volatile uint32_t*)(_r) |= (_b))
#define REG_CLR_BIT(_r, _b)  (*(volatile uint32_t*)(_r) &= ~(_b))




#define LISP_RET_STR_SIZE 256
#define CLEAN_LISP_RET_STR memset(g_lisp_ret, 0, strlen(g_lisp_ret))

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))


#define ASSERT(exp) \
					do{ \
						if(exp) \
						{ \
							u_printf(#exp); \
							goto fail; \
						} \
					}while(0)

#define ASSERT_EVAL(exp, var, code) \
						do{ \
							if(exp) \
							{ \
								u_printf(#exp); \
								var = code; \
								goto fail; \
							} \
						}while(0)

#define __FREE(p) do{	\
						if(p==NULL)	\
							break;	\
						hfmem_free(p),p=NULL;	\
					  }while(0)

#define INT32(ipaddr, a, b, c, d)              \
	(ipaddr) = ((uint32)((a)&0xff) << 24) | \
					 ((uint32)((b)&0xff) << 16) | \
					 ((uint32)((c)&0xff) << 8) |  \
					 (uint32)((d)&0xff)

typedef enum {
		TOKEN_TYPE_RESERVED0 = 0,
		TOKEN_TYPE_RESERVED1 = 1,
		TOKEN_TYPE_DYNAMIC = 2
} token_type_t;

typedef enum {
	ENCRYPT_TYPE_NONE = 0,
	ENCRYPT_TYPE_SSL = 1
}encrypt_type_t;

typedef enum {
	WORK_MODE_TRANSPARENT = 0,
	WORK_MODE_MASTER_CONTROL = 1
}work_mode_t;

typedef enum
{
	HEKR_CONFIG = 0,
	AIRKISS_CONFIG,
	SOFTAP_CONFIG 
}wifi_config_t;

typedef struct {
	char *authkey;
	char *prodkey;
	char *devtid;
	char *devprikey ;
	char *ctrlkey;
	char *bindkey;
	char *token;
	char *feedbackauthkey;
	char *feedbackprodkey;
}device_core_t;

typedef struct {
	char * wechatID;
	char * wechatProductId;
	char * wechatTid;
}airkiss_param_t;

typedef struct
{
	char *ssid;
	char *password;
} wifi_sta_info_t;

typedef struct
{
	char *mid;
	char *sub_devtid;
} sub_device_info_t;

typedef struct {
	device_core_t core;
	char *pin_code;
	char *mid;
	char *service_host;
	int32_t service_port;
	int32_t factory_test;
	wifi_config_t config_type;
	airkiss_param_t airkiss;
	work_mode_t work_mode;
	token_type_t token_type;
	int32_t check;
	encrypt_type_t encrypt_type;
	char *tidProdKey;
	char *mcu_version;
	sub_device_info_t sub_device_info;
	wifi_sta_info_t wifi_sta_info;
	char feedback_exist;
}product_info_t;

typedef struct {
	int32_t remote_port;
	uint8 remote_ip[4];
}remote_info_t;

typedef enum
{
	DATA_FROM_PORT_CLOUD = 0,
	DATA_FROM_PORT_LAN = 1,
	DATA_FROM_PORT_UART = 2
} data_from_port_t;

typedef struct
{
	data_from_port_t port;
	remote_info_t remote;
}port_info_t;

typedef struct tm
{
	int	sec;
	int	min;
	int	hour;
	int	mday;
	int	mon;
	int	year;
	int	wday;
	int	yday;
	int	isdst;
}tm_t;

/****************************************
 * HM define start
 * ************************************/
typedef enum
{
	WIFI_STA_MODE = 0 ,
	WIFI_AP_MODE ,
}WIFI_MODE_T;
typedef enum
{
	UART_INVALID_TYPE =0,
	UART_48_TYPE = 1,
	UART_AT_TYPE = 2,
} HM_UART_MSG_TYPE;

typedef struct 
{
	HM_UART_MSG_TYPE msg_type;
	uint32   msg_lenth;
	char*   msg_data;
} HM_UART_EVENT_MEG_T;

typedef struct
{

	HM_UART_EVENT_MEG_T hm_UART_event_msg;

} HM_RUN_EVENT_PARAMS_T;

typedef enum
{
	RUN_INIT_STATUS,
	RUN_FACTORY_TEST_STATUS,
	RUN_WIFI_CONFIG_STATUS,
	RUN_CLOUD_LINK_STATUS,
	RUN_NORMAL_STATUS,
	RUN_WIFI_TEST_STATUS,
} run_status_t;

typedef enum
{
	FACTORY_TEST_REQ,
	FACTORY_TEST_START,
	FACTORY_TEST_SUCCESS,
	FACTORY_TEST_FAIL,
} FACTORY_TEST_STATUS_T;



typedef struct
{
	run_status_t run_status;
	union 
	{
		FACTORY_TEST_STATUS_T FACTORY_TEST_STATUS;
		cc_step_t  cc_step_status;
	};
}run_info_t;



typedef enum
{
	CLOUD_NOT_LINK,
	CLOUD_LINK_HUB,
	CLOUD_LINK_DEV,
} cloud_link_status_t;
typedef struct
{
	run_info_t run_info;
	HM_LINK_STATUS wifi_link_status;
	cloud_link_status_t cloud_link_status;
	bool factory_test_flag;
}hm_sys_run_info_t;

typedef enum
{
	CLOUD_HEARTBEAT_TIMER_ID = 2,
	UART_TIMER_ID =3,
} TIMER_ID_T;

/****************************
 * 变量声明
 * *************************/
extern char encodeConfigType;
extern product_info_t product_info ;
extern hm_sys_run_info_t hm_sys_run_info ;

/********************************
 * 函数定义
 * ******************************/
char *get_tid(void);
unsigned char *parse_version(const char *vstr, const size_t vstrlen, unsigned char *outlen);


char *get_full_bin_version(void);


char *str_to_hex(char *src, size_t str_len);
char *hex_to_str(char *src, size_t src_len, bool _case);
char *str_to_str(char *src, size_t src_len, bool _case);
void hex_to_str_nomalloc(char *dst, char *src, size_t src_len, bool _case);

void jad_eval(port_info_t *port_info, char *buf, uint16_t len);

char *product_info_get_mcu_version(void);
int product_info_set_mcu_version(const char *mcu_version);
char *product_info_get_mid(void);
int product_info_set_mid(const char *mid);
char *product_info_get_pin_code(void);
int product_info_set_pin_code(const char *pin_code);
char *product_info_get_service_hostname(void);
int product_info_set_service_hostname(const char *service_host);
int32_t product_info_get_service_port(void);
int product_info_set_service_port(int32_t service_port);
char *product_info_get_prodkey(void);
int product_info_set_prodkey(const char *prodkey);
char *product_info_get_devtid(void);
int product_info_set_devtid(const char *devtid);
char *product_info_get_devprikey(void);
int product_info_set_devprikey(const char *devprikey);
int product_info_clear_pincode(void);
char *product_info_get_authkey(void);
int product_info_set_authkey(const char *authkey);
int product_info_clear_authkey(void);
char *product_info_get_token(void);
int product_info_set_token(const char *token);
int product_info_clear_token(void);
char *product_info_get_ctrlkey(void);
int product_info_set_ctrlkey(const char *ctrlkey);
char *product_info_get_bindkey(void);
int product_info_set_bindkey(const char *bindkey);
wifi_config_t product_info_get_config_type(void);
int product_info_set_config_type(wifi_config_t config);
int32_t product_info_get_factory_test(void);
int product_info_set_factory_test(int32_t flag);

char *product_info_get_config_ssid(void);
int product_info_set_config_ssid(const char *ssid);
char *product_info_get_config_password(void);
int product_info_set_config_password(const char *password);
signed char hm_wifi_station_get_rssi(void);

void device_detail_print(void);

int product_info_set_work_mode(work_mode_t work_mode);
work_mode_t product_info_get_work_mode(void);
int product_info_set_encrypt_type(encrypt_type_t encrypt_type);
encrypt_type_t product_info_get_encrypt_type(void);
int product_info_set_token_type(token_type_t token_type);
token_type_t product_info_get_token_type(void);
char *get_mac_string(void);

char *station_get_current_ssid(void);
bool wifi_station_config_is_exist(void);

extern void hm_wifi_get_macaddr(uint8_t *mac) ;
extern uint8 hm_get_wifi_mode(void);
extern uint8 hm_get_wifi_status(void);
extern uint8 hm_get_cloud_connect_status(void);
extern uint8 hm_get_signal_qual(void);
extern uint8 hm_get_device_test_status(void);

extern void hm_product_info_load(product_info_t *info);
extern int hm_sys_run_info_init(void);

char *station_get_ip_string(void);
WIFI_MODE_T hm_get_wifi_work_mode(void);
void hm_set_wifi_work_mode(WIFI_MODE_T wifimode, bool treboot);

void hm_restory_factory_data(product_info_t *info);
void hm_restory_app_data(product_info_t *info);
void hm_restory_all_data(product_info_t *info);
#endif