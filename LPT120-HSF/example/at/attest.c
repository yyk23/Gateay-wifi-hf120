
#include "hsf.h"


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


static int hf_atcmd_test(pat_session_t s,int argc,char *argv[],char *rsp,int len)
{
	char flash_data[33];
	
	if(argc == 0)
	{
		memset(flash_data, 0, 33);
		hfuflash_read(0, (uint8_t *)flash_data, 32);
		sprintf(rsp, "=%s", flash_data);
		return 0;
	}
	else if(argc == 1)
	{
		hfuflash_erase_page(0, 1);
		hfuflash_write(0, (uint8_t *)argv[0], 32);
	}
	
	return 0;
	
}

const hfat_cmd_t user_define_at_cmds_table[]=
{
	{"TEST",hf_atcmd_test,"   AT+TEST: test At cmd.\r\n",NULL},	
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

	char *words[6]={NULL};
	char rsp[64]={0};
	hfat_send_cmd("AT+WMODE\r\n",sizeof("AT+WMODE\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 6)>0)
	{
		u_printf("\nAT+WMODE=%s\r\n",words[1]);
	}
	
	hfat_send_cmd("AT+WSSSID\r\n",sizeof("AT+WSSSID\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 6)>0)
	{
		u_printf("\nAT+WSSSID=%s\r\n",words[1]);
	}
	
	hfat_send_cmd("AT+WSKEY\r\n",sizeof("AT+WSKEY\r\n"),rsp,64);
	if(hfat_get_words(rsp,words, 6)>0)
	{
		u_printf("\nAT+WSKEY=%s,%s,%s\r\n",words[1],words[2],words[3]);
	}
	
	return 1;
}


