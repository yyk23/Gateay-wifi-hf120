
#ifndef __HF_CONFIG____
#define __HF_CONFIG____


#pragma pack(push)
#pragma pack(1)

typedef struct
{
	uint8_t		mode; //out 0:0  out 1:1  pwm : 2 input:3
	uint8_t		duty;
	uint16_t	freq;
}DEV_PIN;

typedef struct
{
	int8_t     	updateflag; 	//update flag
	int8_t     	validflag;		//valid flag
	int8_t		tmode_mode;	    //AT+TMODE,throughput=0,cmd=1,gpio=2,pwm=3
	
	int8_t     	wmode_mode;  	//AT+WMODE ,AP=0,STA=1,AP+STA=2
	uint8_t 	sta_ssid[33];		//AT+WSSSID
	
	int8_t     	wap_mode;       	//AT+WAP,11b=0,11bg=2,11bgn=2
	char 		ap_ssid[33];  	//AT+WAP
	int8_t      wap_channel;    	//AT+WAP,ch1=1,ch2=2,бн,ch14=14
	
	int8_t     	waauth;         	//AT+WAKEY
	int8_t     	waencry;        	//AT+WAKEY 
	uint8_t 	ap_key[65];  
	
	int8_t     	wsauth;       		//AT+WSKEY
	int8_t     	wsencry;        	//AT+WSKEY
	uint8_t 	sta_key[65];
	
	int8_t     	baudrate;       	//AT+UART	9600,57600,115200...
	int8_t     	data_bits;		//AT+UART	5, 6,7, 8	
	int8_t     	stop_bits;		//AT+UART	1, 1.5, 2
	int8_t     	parity;			//AT+UART	none, odd, even, mark, space
	
	int8_t	    ctsrts;			//AT+UART
	uint8_t    	maxsocket; 		//AT+MAXSK
    uint16_t    tcpto;    		//AT+TCPTO 
	
	int8_t     	protocol;		//AT+NETP	TCP: 0, UDP: 1
	int8_t    	app_mode;		//AT+NETP   server mode: 0, client mode: 1
	uint16_t	pro_port;	 	//AT+NETP
	char 		cipadd[101];		//AT+NETP
	
	int8_t     	uartfable;   		//AT+UARTF
	int8_t     	fuartte_mode;  	//AT+FUARTTE
    uint16_t	uartft;  			//AT+UARTFT
    uint16_t	uartfl;   			//AT+UARTFL   
	
	/*AT+WANN*/	
	uint8_t		wann_mode;   		//AT+WANN
	char		wann_ipaddr[16];	//AT+WANN
	char		wann_mask[16]; 		//AT+WANN    
	char		wann_gateway[16];	//AT+WANN  

	/*AT+LANN*/
	char     	lann_ipaddr[16];		//AT+LANN
	char	    lann_mask[16];		//AT+LANN            
	
    int8_t      rlden;				//AT+RLDEN   
	int8_t		txpwr;				//AT+TXPWR
	int8_t		dhcpsw;				//AT+DHCP-new

	uint8_t		mid[21];
	uint8_t		aswd[21];
	
	/*for sleep momde*/
	int8_t     	bsleep;   			//AT+MSLP	normal,deepsleep,standby
	int8_t     	msopt;    			//AT+MSOPT	  
	uint16_t    tspartimeout;  		//AT+TSPAR
	
	int8_t      reset;				//AT+RESET            
	int8_t		web_switch;			//record the recv

	char		user_name[16];	
	char		user_password[16];	

	uint32_t 	dns_addr;			//dns server IP

	uint8_t		web_language;		//for choose web language
	uint8_t		domain_name[21];	//save domain name

	/*AT+UPURL*/
	char		update_url[101];	//save domain name
	char		update_file[21];

	int32_t		firmware_size;
	int8_t		sleep_mode;
	int8_t		echo;
	int8_t		slpen;
	int8_t		uplogsw;
	uint16_t	uplogport;
	uint16_t	prst;
	uint16_t	crst;

	int8_t		modechange;
	int8_t		connat;
	int8_t		lpt_io;
	int8_t		update_pin;
	uint8_t     debug_level;
	uint8_t     wps_btn_enable;
	uint8_t     walk_led;
	uint8_t	    dis_power_saving;

	uint16_t    sockb_tcpto;    		//AT+TCPTO 
	int8_t     	sockb_pro;		// NONE: 0, TCP: 1, UDP: 2
	uint16_t	sockb_port;
	char	 	sockb_cipadd[101];
	
	uint8_t     pmk_av;			// PMK availible, 0: not availble, 1:availible      
	uint8_t		pmk[32];			// PMK value
	uint8_t     ap_pmk_av;			// PMK availible, 0: not availble, 1:availible      
	uint8_t		ap_pmk[32];			// PMK value
	DEV_PIN		gpio_config[6];
	
	uint8_t		http_type;// 0 get, i post
	char		http_path[50];
	char		http_useragent[20];
	uint8_t		http_connec[20];

	uint8_t		ntp_time;
	uint8_t		ntp_stats;

	uint8_t     dhcpd_addr_start;
	uint8_t     dhcpd_addr_end;
	int8_t      uart1_baudrate;
	int8_t     	uart1_data_bits;
	int8_t     	uart1_stop_bits;
	int8_t     	uart1_parity;
	int8_t      uart1_debug;

	uint32_t	ntpserver;
	uint16_t	udp_localporta;
	uint16_t	udp_localportb;
	uint8_t     smtlk_mode;
	uint8_t     smtlk_protocol;
	uint8_t     connect_flag;
	uint8_t     sta_bssid[6];
	uint8_t     sta_channel;
	uint8_t 	enable_dcdc;
	uint8_t     max_sta_num;
	uint8_t     ps_interval;
	uint8_t		reserved1[72];
	int8_t     	validflag_end;		//valid flag
}HF_CONFIG_FILE;

#define HF_CONFIG_STA_CONNECT_FLAG_CONNNECT_OK			(2)

typedef struct
{
	uint32_t magic_code;
	uint32_t ver;
	uint32_t ota_flag;
	uint32_t run_mode;
	uint8_t  smartlink_type;
	uint8_t  airkiss_rand_data;
}HF_BOOT_CONFIG;

#pragma pack(pop)

void hf_config_update_connect_flag(uint8_t flag);


extern HF_CONFIG_FILE  g_hf_config_file;
extern HF_BOOT_CONFIG  g_hf_boot_config;

#define HF_CONFIG_OTA_FLAG_REDAY		(0x0A0A55A0)
#define HF_CONFIG_OTA_FLAG_OK			(0x0505AA50)
#define HF_BOOT_CONFIG_MAGIC_CODE		(0x48464254)
#define HF_CONFIG_OTA_FLAG_FAIL			(0x5A5A5A00)

void hf_boot_update_ota_flag(uint32_t flag);
void hf_boot_update_run_mode(uint32_t mode);

int hf_boot_config_save(void);

#define HF_BOOT_RUN_MODE_SMARTLINK_START		(0xA5010200)
#define HF_BOOT_RUN_MODE_SMARTLINK_OK			(0xA5010201)
#define HF_BOOT_RUN_MODE_NORMAL					(0)
#define HF_BOOT_RUN_MODE_AUDO_UPGRADE			(0xA5010202)
#define HF_BOOT_RUN_MODE_DEBUG					(0xA5010203)


enum ENWAAUTH
{
	WAAUTH_OPEN=0,
	WAAUTH_SHARED=1,
	WAAUTH_WPAPSK,
	WAAUTH_WPA2PSK,
	WAAUTH_WPAPSKWPA2PSK
};

enum ENWAENCRY
{
	WAENCRY_NONE=0,
	WSENCRY_WEP_A=1,
    WSENCRY_WEP_H,
	WAENCRY_TKIP,
	WAENCRY_AES,
	WAENCRY_TKIPAES,
	WSENCRY_WEP
};

enum ENNETSETADD
{
	NETSETADD_HOST		=0x00,
	NETSETADD_IP
};

enum ENNETSETAPP
{
	NETSETAPP_SERVER		=0x00,
	NETSETAPP_CLIENT
};

enum ENNETSETPRO
{
	NETSETPRO_TCP		=0x00,
	NETSETPRO_UDP
};

typedef enum ENBAUDRATE
{
	BAUDRATE_50							= 0x00,
	BAUDRATE_75,
	BAUDRATE_110,
	BAUDRATE_134,
	BAUDRATE_150,
	BAUDRATE_200,
	BAUDRATE_300,
	BAUDRATE_600,
	BAUDRATE_1200,
	BAUDRATE_1800,
	BAUDRATE_2400,
	BAUDRATE_4800,
	BAUDRATE_9600,
	BAUDRATE_19200,
	BAUDRATE_38400,
	BAUDRATE_57600,
	BAUDRATE_115200,
	BAUDRATE_230400,
	BAUDRATE_380400,
	BAUDRATE_460800,
	BAUDRATE_921600
}ENBAUDRATE_E;

typedef enum ENCOMBITS
{
	COMBITS_5     = 0x00,
	COMBITS_6,
	COMBITS_7,
	COMBITS_8
}ENCOMBITS_E;

enum ENCOMDEVICE
{
	COMDEVICE_COM0	= 0x00,
	COMDEVICE_COM1
};

typedef enum ENCOMPARITY
{
	COMPARITY_NONE	= 0x00,
	COMPARITY_ODD,
	COMPARITY_EVEN,
	COMPARITY_MARK,
	COMPARITY_SPACE
}ENCOMPARITY_E;

typedef enum ENCOMSTOPBITS
{
	COMSTOPBITS_1	= 0x00,
	COMSTOPBITS_1P5,
	COMSTOPBITS_2
}ENCOMSTOPBITS_E;

enum ENCOMUARTCTL
{
	COMUARTFC	= 0x00,
	COMUARTNFC
};

enum ENCOMWANNMODE
{
	COMWANNDHCP	= 0x00,
	COMWANNSTATIC
};

enum ENUARTFABLE
{
	HFUARTFDISABLE	= 0x00,
	HFUARTFENABLE,
	HFUARTHFFORMAT,
};

enum ENUARTFMODE
{
	M2M_UARTTE_FAST	= 0x00,
	M2M_UARTTE_NORMAL
};

enum CONFIG_WAMOD
{
	CONFIG_WAMOD_11B	= 0x00,
	CONFIG_WAMOD_11BG,
	CONFIG_WAMOD_11BGN
};

enum CONFIG_WMODE
{
	CONFIG_WMODE_AP	= 0x00,
	CONFIG_WMODE_STA,
	CONFIG_WMODE_APSTA
};

enum ENWORKTMODE
{
	M2M_STATE_RUN_THROUGH,
	M2M_STATE_RUN_CMD,
	M2M_STATE_RUN_GPIO,
	M2M_STATE_RUN_PWM
};

enum CONFIG_PSLP
{
	CONFIG_PSLP_NORMAL	= 0x00,
	CONFIG_PSLP_STANDBY,
	CONFIG_PSLP_DEEPSLEEP
};

enum CONFIG_PRLDEN
{
	CONFIG_PRLDEN_DISABLE	= 0x00,
	CONFIG_PRLDEN_ENABLE
};

enum CONFIG_WADHCP
{
	CONFIG_WADHCP_DISABLE	= 0x00,
	CONFIG_WADHCP_ENABLE
};

enum CONFIG_WEBLANGUAGE
{
	CONFIG_WEBLAN_EN	= 0x00,
	CONFIG_WEBLAN_CN
};

enum CONFIG_SLEEPMODE
{
	CONFIG_WMODE_NORMAL	= 0x00,
	CONFIG_WMODE_SLEEP
};

enum CONFIG_GPIO_SEL
{
	CONFIG_GPIO_PA3 = 0x00,
	CONFIG_GPIO_PA8,
	CONFIG_GPIO_PA9
};

enum CONFIG_PWM_CHANNEL
{
	CONFIG_PWM_CHANNEL0 = 0,
	CONFIG_PWM_CHANNEL1,
	CONFIG_PWM_CHANNEL2,
	CONFIG_PWM_CHANNEL3
};

enum CONFIG_PWMGPIO
{
	CONFIG_MODE_PWM = 0x00,
	CONFIG_MODE_GPIO
};

enum CONFIG_GPIO_WAY
{
	CONFIG_GPIO_SET = 20,
	CONFIG_GPIO_OUT,
	CONFIG_GPIO_GET,
	CONFIG_GPIO_IN
};

enum MODULE_ERRNUM
{
	UART_THREAD_ERROR = 1,
	TX_THREAD_ERROR,
	SDIOIRQ_THREAD_ERROR,
	TCPIP_THREAD_ERROR,
	WEB_THREAD_ERROR,
	SOCK_THREAD_ERROR,
	PRST_THREAD_ERROR,
	CRST_THREAD_ERROR,
	PBUF_TIME_ERROR,
	PBUF_NUM_ERROR,
	SDIO_NUM_ERROR,
	POLLING_NUM_ERROR,
	EVENT_SEND_ERROR,
	SOCKB_THREAD_ERROR,
	STA_CONNECT_ERROR,
	PING_SDIO_ERROR,
	STA_CONNECT_FAILED,
	SOCK_SEND_FAILED,
	APSTA_FIND_ROUTER,
	APSTA_STA_DISCONNECT,
	APSTA_CHANNEL_ERROR,
	STA_SCAN_NOAP,
	TRAPFAULT_ERROR,
	SDIO_INIT_ERROR,
	LINK_STATUS_ERROR,
	SOCKETAB_TCP_SND_ERROR,
	ARP_CHECK_ERROR,
	RECV_INVALID_PKTS,
	WIFI_START_ERROR,
};

enum ENGPIOCODE
{
	GPIO_OUTPUT0							=0,
	GPIO_OUTPUT1,
	GPIO_PWM,
	GPIO_INPUT
};

enum CONFIG_DISPS
{	
	CONFIG_AUTO_PS = 0x00,
	CONFIG_EN_PS	= 0x01,
	CONFIG_DIS_PS   = 0x02,
};

#define UPDATEFLAG 				2// 1

extern int g_module_id;
extern const char *g_hfm_name[];

int uart_baud_num2en(int num, int *en);

void get_uart_baudrate(int *nBaud,int uart_no);

void get_uart_bits(int *bits,int uart_no);

void get_uart_stop(int *stop,int uart_no);

void get_uart_parity_str(char *parity_str,int uart_no);

void get_uart_fc(char *fc,int uart_no);

int setbaudrate(int baud,int uart_no);

int setdatabits(int bits,int uart_no);

int setstopbits(int stop,int uart_no);

int setparity(int parity,int uart_no);

void hf_config_file_save(void);

void hf_config_file_init(void);

void hf_config_file_reload(void);

int  hf_config_write_default_rf_table(void);

int hfconfig_fseting_save(void);

void hf_config_enable_save(bool enable);

int hfconfig_fseting_save_ex(HF_CONFIG_FILE *file);

#endif


