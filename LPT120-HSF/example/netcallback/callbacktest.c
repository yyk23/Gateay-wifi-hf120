
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


const hfat_cmd_t user_define_at_cmds_table[]=
{
	{NULL,NULL,NULL,NULL}
};

static hftimer_handle_t hnlink_timer=NULL;
#define NLINK_FALSH_TIMER_ID		(1)
void USER_FUNC nlink_falsh_timer_callback( hftimer_handle_t htimer )
{
	if(hftimer_get_timer_id(htimer)==NLINK_FALSH_TIMER_ID)
	{
		if(hfgpio_fpin_is_high(HFGPIO_F_NLINK))
			hfgpio_fset_out_low(HFGPIO_F_NLINK);
		else
			hfgpio_fset_out_high(HFGPIO_F_NLINK);
		//hftimer_start(htimer);//如果create的时候auto_reload设置为false，手动再次启动timer
	}
}

static int USER_FUNC uart_recv_callback(uint32_t event,char *data,uint32_t len,uint32_t buf_len)
{
	u_printf("[%d]uart recv %d bytes data %d\n",event,len,buf_len);

	data[len]=0;
	if(strcasecmp("GPIO NLINK LOW",data)==0)
	{
		hftimer_stop(hnlink_timer);
		hfgpio_fset_out_low(HFGPIO_F_NLINK);
	}
	else if(strcasecmp("GPIO NLINK HIGH",data)==0)
	{
		hftimer_stop(hnlink_timer);
		hfgpio_fset_out_high(HFGPIO_F_NLINK);
	}
	else if(strcasecmp("GPIO NLINK FLASH",data)==0)
	{
		hftimer_start(hnlink_timer);
	}
	else
		return len;
		
	return 0;
}


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
	if(hfnet_start_uart(0,(hfnet_callback_t)uart_recv_callback) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
#else
	if(hfnet_start_uart(0,(hfnet_callback_t)uart_recv_callback) != HF_SUCCESS)
	{
		HF_Debug(DEBUG_WARN,"start uart fail!\n");
	}
#endif

	//创建一个自动定时器，每1s钟触发一次。
	if((hnlink_timer = hftimer_create("NLINK-FALSH-TIMER",1000,true,1,nlink_falsh_timer_callback,0))==NULL)
	{
		u_printf("create timer fail\n");
	}
	
	return 1;
}


