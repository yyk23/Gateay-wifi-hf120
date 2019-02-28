
#include "hsf.h"

PROCESS(wifi_scan_test_process, "wifi_scan_test_process");
PROCESS(sleep_test_process, "sleep_test_process");


int g_module_id = __HF_MODULE_ID__;

#if (__HF_MODULE_ID__==HFM_LPB120)

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HFM_NOPIN,	//HFGPIO_F_JTAG_TCK
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDO
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDI
	HFM_NOPIN,	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HFM_GPIO20,	//HFGPIO_F_UART0_TX
	HFM_NOPIN,	//HFGPIO_F_UART0_RTS
	HFM_GPIO19,	//HFGPIO_F_UART0_RX
	HFM_NOPIN,	//HFGPIO_F_UART0_CTS
	
	HFM_NOPIN,	//HFGPIO_F_SPI_MISO
	HFM_NOPIN,	//HFGPIO_F_SPI_CLK
	HFM_NOPIN,	//HFGPIO_F_SPI_CS
	HFM_NOPIN,	//HFGPIO_F_SPI_MOSI
	
	HFM_GPIO5,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_GPIO6,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HFM_GPIO15,	//HFGPIO_F_NLINK
	HFM_GPIO18,	//HFGPIO_F_NREADY
	HFM_GPIO2,	//HFGPIO_F_NRELOAD
	HFM_NOPIN,		//HFGPIO_F_SLEEP_RQ
	HFM_NOPIN,		//HFGPIO_F_SLEEP_ON
	
	HFM_NOPIN,		//HFGPIO_F_WPS
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};

#elif  (__HF_MODULE_ID__==HFM_LPT220)

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HFM_NOPIN,	//HFGPIO_F_JTAG_TCK
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDO
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDI
	HFM_NOPIN,	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HFM_GPIO20,	//HFGPIO_F_UART0_TX
	HFM_NOPIN,	//HFGPIO_F_UART0_RTS
	HFM_GPIO19,	//HFGPIO_F_UART0_RX
	HFM_NOPIN,	//HFGPIO_F_UART0_CTS
	
	HFM_NOPIN,	//HFGPIO_F_SPI_MISO
	HFM_NOPIN,	//HFGPIO_F_SPI_CLK
	HFM_NOPIN,	//HFGPIO_F_SPI_CS
	HFM_NOPIN,	//HFGPIO_F_SPI_MOSI
	
	HFM_GPIO5,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_GPIO6,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HFM_GPIO15,	//HFGPIO_F_NLINK
	HFM_GPIO3,	//HFGPIO_F_NREADY
	HFM_GPIO2,	//HFGPIO_F_NRELOAD
	HFM_NOPIN,		//HFGPIO_F_SLEEP_RQ
	HFM_NOPIN,		//HFGPIO_F_SLEEP_ON
	
	HFM_NOPIN,		//HFGPIO_F_WPS
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};

#elif (__HF_MODULE_ID__ == HFM_LPT120)

const int hf_gpio_fid_to_pid_map_table[HFM_MAX_FUNC_CODE]=
{
	HFM_NOPIN,	//HFGPIO_F_JTAG_TCK
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDO
	HFM_NOPIN,	//HFGPIO_F_JTAG_TDI
	HFM_NOPIN,	//HFGPIO_F_JTAG_TMS
	HFM_NOPIN,		//HFGPIO_F_USBDP
	HFM_NOPIN,		//HFGPIO_F_USBDM
	HFM_GPIO20,	//HFGPIO_F_UART0_TX
	HFM_NOPIN,	//HFGPIO_F_UART0_RTS
	HFM_GPIO19,	//HFGPIO_F_UART0_RX
	HFM_NOPIN,	//HFGPIO_F_UART0_CTS
	
	HFM_NOPIN,	//HFGPIO_F_SPI_MISO
	HFM_NOPIN,	//HFGPIO_F_SPI_CLK
	HFM_NOPIN,	//HFGPIO_F_SPI_CS
	HFM_NOPIN,	//HFGPIO_F_SPI_MOSI
	
	HFM_GPIO5,	//HFGPIO_F_UART1_TX,
	HFM_NOPIN,	//HFGPIO_F_UART1_RTS,
	HFM_GPIO6,	//HFGPIO_F_UART1_RX,
	HFM_NOPIN,	//HFGPIO_F_UART1_CTS,
	
	HFM_GPIO15,	//HFGPIO_F_NLINK
	HFM_GPIO3,	//HFGPIO_F_NREADY
	HFM_GPIO2,	//HFGPIO_F_NRELOAD
	HFM_NOPIN,		//HFGPIO_F_SLEEP_RQ
	HFM_NOPIN,		//HFGPIO_F_SLEEP_ON
	
	HFM_NOPIN,		//HFGPIO_F_WPS
	HFM_NOPIN,		//HFGPIO_F_RESERVE1
	HFM_NOPIN,		//HFGPIO_F_RESERVE2
	HFM_NOPIN,		//HFGPIO_F_RESERVE3
	HFM_NOPIN,		//HFGPIO_F_RESERVE4
	HFM_NOPIN,		//HFGPIO_F_RESERVE5
	
	HFM_NOPIN,	//HFGPIO_F_USER_DEFINE
};
#else
#error "Please define __HF_MODULE_ID__!!!"
#endif


static int hfwifi_scan_test( PWIFI_SCAN_RESULT_ITEM scan_ret)
{
	if(scan_ret == NULL)
		return 0;
	
	int i;
	u_printf("%s,", scan_ret->ssid);	

	switch(scan_ret->auth)
	{
		case AUTH_TYPE_OPEN:
			u_printf("%s,", "OPEN");
			break;

		case AUTH_TYPE_SHARED:
			u_printf("%s,", "SHARED");
			break;

		case AUTH_TYPE_WPAPSK:
			u_printf("%s,", "WPAPSK");
			break;

		case AUTH_TYPE_WPA2PSK:
			u_printf("%s,", "WPA2PSK");
			break;

		case AUTH_TYPE_WPAPSKWPA2PSK:
			u_printf("%s,", "WPAPSKWPA2PSK");
			break;

		default:
			u_printf(",");
			break;
	}

	switch(scan_ret->encry)
	{
		case ENC_TYPE_NONE:
			u_printf("%s,", "NONE");
			break;

		case ENC_TYPE_WEP:
			u_printf("%s,", "WEP");
			break;

		case ENC_TYPE_TKIP:
			u_printf("%s,", "TKIP");
			break;

		case ENC_TYPE_AES:
			u_printf("%s,", "AES");
			break;

		case ENC_TYPE_TKIPAES:
			u_printf("%s,", "TKIPAES");
			break;
			
		default:
			u_printf(",");
			break;
	}

	u_printf("%d,", scan_ret->channel);
	u_printf("%d,", scan_ret->rssi);
	u_printf("");
	for(i=0; i<6; i++)
		u_printf("%X", ((uint8_t*)scan_ret->mac)[i]);
	u_printf("\r\n");
	return 0;
}

PROCESS_THREAD(sleep_test_process, ev, data)
{
	PROCESS_BEGIN();
	static struct etimer timer_sleep;

	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_CONTINUE);
		if(ev == PROCESS_EVENT_CONTINUE)
		{
			etimer_set(&timer_sleep, 30 * CLOCK_SECOND);
			continue;
		}
		
		process_post(&wifi_scan_test_process, PROCESS_EVENT_CONTINUE, NULL);
		etimer_stop(&timer_sleep);
	}
	
	PROCESS_END();
}

PROCESS_THREAD(wifi_scan_test_process, ev, data)
{
	PROCESS_BEGIN();
	static struct etimer timer_sleep;

	etimer_set(&timer_sleep, 5 * CLOCK_SECOND);
	while(1) 
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER ||ev ==PROCESS_EVENT_CONTINUE);
		
		u_printf("ssid,auth,encry,channel,rssi,mac\r\n\r\n");
		hfwifi_scan(hfwifi_scan_test);
		
		//sleep 30s
		process_post_synch(&sleep_test_process, PROCESS_EVENT_CONTINUE, NULL);
	}
	
	PROCESS_END();
}

const hfat_cmd_t user_define_at_cmds_table[]=
{
	{NULL,NULL,NULL,NULL}
};


int USER_FUNC  app_main(void)
{
	u_printf("\n%s Start %s %s\n\n",g_hfm_name[__HF_MODULE_ID__],__DATE__,__TIME__);

	if(hfnet_start_assis(ASSIS_PORT) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start httpd fail\n");
	}

#ifdef SUPPORT_UART_THROUGH
	if(hfnet_start_socketa(0,NULL) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start socketa fail\n");
	}
	
	if(hfnet_start_socketb(1,NULL) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start socketb fail\n");
	}
	if(hfnet_start_uart_ex(0,NULL,4096) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
#else
	if(hfnet_start_uart_ex(0,NULL,1024) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
#endif

	process_start(&wifi_scan_test_process, NULL);
	process_start(&sleep_test_process, NULL);
	
	return 1;
}


