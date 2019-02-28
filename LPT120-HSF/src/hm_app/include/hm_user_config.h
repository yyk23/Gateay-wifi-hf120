#ifndef __HM_USER_CONFIG_H__
#define __HM_USER_CONFIG_H__

#ifndef __USEVS__
//#define USE_OPTIMIZE_PRINTF
#endif

#if defined(__DEBUG__)&& defined(__ESP8266__)
//#define __MEM_LEAK_CHECK_ENABLE__
#endif

#define SYSTEM_RESERVE_HEAP_SIZE	4096	//byte

#define WIFI_SCAN_BUF_SIZE	(2048U)			//byte

#define FIRMWARE_MAX_SIZE (492*1024U)		//byte

#define HOST_FILE_PATH		"/host"
#define ACCESSKEY_FILE_PATH "/accesskey"
#define TEST_FLAG_FILE_PATH	"/testflag"
#define MID_FILE_PATH		"/mid"
#define	PID_FILE_PATH		"/pid"
#define	CID_FILE_PATH		"/cid"
#define WIFI_CONFIG_PATH	"/wificonfig"

#define MAIN_DOMAIN	"hekr.me"

#define NORMALSEVER

#if defined(NORMALSEVER)

#define DEFULT_DOMAIN  "device.hekr.me"
#elif defined(WEIXIN_SERVER)
#define DEFULT_DOMAIN  "weixin-device.smartmatrix.mx"
#endif

#define FACTORY_TEST_DOMAIN  "factorytest.hekr.me"

#define DEFULT_SERVER_PORT	83

#if defined(__PLTC_DJ_LAMPAIR__)
#define UART_DEFAULT_BAUDRATE		 115200
#else
#define UART_DEFAULT_BAUDRATE		 9600
#endif

#define DEFAULT_NEWWORK_LED_PIN		4

//#define LISP_UART_CALLBACK


#define BITMAP_W					32    //bitmap width
#define SET_BIT_0(bitmap,n)			(bitmap)&=~(1U<<n)
#define SET_BIT_1(bitmap,n)			(bitmap)|=(1U<<n)
#define SET_BITMAP_0(bitmap,set)	(bitmap&(~set))


#ifndef GCC_VERSION 
#if defined(__GNUC__)
#define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#endif
#endif

#ifndef expect
#if (defined(GCC_VERSION) && (GCC_VERSION >= 302)) || defined(__clang__)
#define expect(expr,value) (__builtin_expect ((expr),(value)) )
#else
#define expect(expr,value) (expr)
#endif
#endif

#ifndef likely
#define likely(expr) expect((expr) != 0, 1)
#endif

#ifndef unlikely
#define unlikely(expr) expect((expr) != 0, 0)
#endif


#define FILLING_STR_32	"12345678123456781234567812345678"
#define UINT8_MAX_STR	"256"
#define UINT32_MAX_STR	"4294967296"
#define INT32_MAX_STR	"+2147483647"

//#ifndef UINT32_MAX
//#define UINT32_MAX       0xffffffffU
//#endif // !_STDINT


/*interrupt*/
#define GEMINI_IRQ_NUM_GPIO0		0
#define GEMINI_IRQ_NUM_GPIO_MAX		15
#define GEMINI_IRQ_NUM_MAX			16

typedef enum 
{
    TIMER_UART_RECV_ID = 0,
    TIMER_HEARTBEAT_ID
} hm_timer_id;

#endif