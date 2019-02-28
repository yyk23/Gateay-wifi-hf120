
/* hfgpio.h
 *
 * Copyright (C) 2013-2020 ShangHai High-flying Electronics Technology Co.,Ltd.
 *				  	All rights reserved.
 *
 * This file is part of HSF.
 *
 *
 */

#ifndef _HF_GPIO_H_H_H_
#define _HF_GPIO_H_H_H_

typedef void (*hfgpio_interrupt_func)(uint32_t,uint32_t);
//gpio function define

#define HFM_PIN_NUMBER		(48+3)

#define HFM_TYPE_LPT120		0


#define HFM_MAX_FUNC_CODE	(HFM_PIN_NUMBER*3)

enum HF_GPIO_FUNC_E
{
	//////fix/////////////////////
	HFGPIO_F_JTAG_TCK=0,
	HFGPIO_F_JTAG_TDO=1,
	HFGPIO_F_JTAG_TDI,
	HFGPIO_F_JTAG_TMS,
	HFGPIO_F_USBDP,
	HFGPIO_F_USBDM,
	HFGPIO_F_UART0_TX,
	HFGPIO_F_UART0_RTS,
	HFGPIO_F_UART0_RX,
	HFGPIO_F_UART0_CTS,
	HFGPIO_F_SPI_MISO,
	HFGPIO_F_SPI_CLK,
	HFGPIO_F_SPI_CS,
	HFGPIO_F_SPI_MOSI,
	HFGPIO_F_UART1_TX,
	HFGPIO_F_UART1_RTS,
	HFGPIO_F_UART1_RX,
	HFGPIO_F_UART1_CTS,
	////////////////////////////////
	HFGPIO_F_NLINK,
	HFGPIO_F_NREADY,
	HFGPIO_F_NRELOAD,
	HFGPIO_F_SLEEP_RQ,
	HFGPIO_F_SLEEP_ON,
	HFGPIO_F_WPS,
	HFGPIO_F_IR,
	HFGPIO_F_RESERVE2,
	HFGPIO_F_RESERVE3,
	HFGPIO_F_RESERVE4,
	HFGPIO_F_RESERVE5,
	HFGPIO_F_USER_DEFINE
};

#define F_GPI			(0x00010000)
#define F_GPO			(0x00020000)
//GND
#define F_GND			(0x00040000)
//use to Peripherals interface
#define F_PI			(0x00080000)
//vcc
#define F_VCC			(0x00100000)
#define F_NC			(0x00200000)
//use to system reset
#define F_RST			(0x00400000)
//use to interrupt input pin
#define F_IT			(0x00800000|F_GPI)

#define F_GPIO			(F_GPI|F_GPO)

#define F_PWM			(0x01000000)

#define F_ADC			(0x02000000)

#define HFM_NOPIN			(0)
#define HFM_PIN1			(F_NC|1)
#define HFM_PIN2			(F_NC|2)
#define HFM_PIN3			(F_VCC|3)
#define HFM_PIN4			(F_VCC|4)
#define HFM_PIN5			(F_PI|5)
#define HFM_PIN6			(F_PI|6)
#define HFM_PIN7			(F_PI|7)
#define HFM_PIN8			(F_PI|8)
#define HFM_PIN9			(F_PI|9)
#define HFM_PIN10			(F_PI|10)
#define HFM_PIN11			(F_PI|11)
#define HFM_PIN12			(F_NC|12)
#define HFM_PIN13			(F_NC|13)
#define HFM_PIN14			(F_GPIO|F_IT|14)
#define HFM_PIN15			(F_VCC|15)
#define HFM_PIN16			(F_GPIO|F_IT|16)
#define HFM_PIN17			(F_VCC|17)
#define HFM_PIN18			(F_GPIO|F_IT|18)
#define HFM_PIN19			(F_GPIO|F_IT|19)
#define HFM_PIN20			(F_VCC|20)
#define HFM_PIN21			(F_VCC|21)
#define HFM_PIN22			(F_VCC|22)
#define HFM_PIN23			(F_VCC|23)
#define HFM_PIN24			(F_NC|24)
#define HFM_PIN25			(F_NC|25)
#define HFM_PIN26			(F_VCC|26)
#define HFM_PIN27			(F_VCC|27)
#define HFM_PIN28			(F_VCC|28)
#define HFM_PIN29			(F_PI|29)
#define HFM_PIN30			(F_PI|30)
#define HFM_PIN31			(F_PI|31)
#define HFM_PIN32			(F_PI|32)
#define HFM_PIN33			(F_PI|33)
#define HFM_PIN34			(F_PI|34)
#define HFM_PIN35			(F_GPIO|35)
#define HFM_PIN36			(F_VCC|36)
#define HFM_PIN37			(F_GPIO|F_IT|37)
#define HFM_PIN38			(F_GPIO|F_IT|38)
#define HFM_PIN39			(F_GPIO|F_IT|39)
#define HFM_PIN40			(F_GPIO|F_IT|F_PI|40)
#define HFM_PIN41			(F_GPIO|F_IT|F_PI|41)
#define HFM_PIN42			(F_VCC|42)
#define HFM_PIN43			(F_VCC|43)
#define HFM_PIN44			(F_NC|44)
#define HFM_PIN45			(F_PI|45)
#define HFM_PIN46			(F_NC|46)
#define HFM_PIN47			(F_NC|47)
#define HFM_PIN48			(F_NC|48)
#define HFM_PIN49			(F_GPIO|49)
#define HFM_PIN50			(F_GPIO|50)
#define HFM_PIN51			(F_GPIO|51)

#define HFM_GPIO1			HFM_PIN16
#define HFM_GPIO2			HFM_PIN18
#define HFM_GPIO3			HFM_PIN19
#define HFM_GPIO5           HFM_PIN35
#define HFM_GPIO6			HFM_PIN37
#define HFM_GPIO8			HFM_PIN14
#define HFM_GPIO15			HFM_PIN38
#define HFM_GPIO18			HFM_PIN39	
#define HFM_GPIO19			HFM_PIN40
#define HFM_GPIO20			HFM_PIN41
#define HFM_GPIO25			HFM_PIN49
#define HFM_GPIO26			HFM_PIN50
#define HFM_GPIO27			HFM_PIN51


#define HF_M_PINNO(_pin)	((_pin)&0xFF)

#define HFM_VALID_PINNO(_pinno)	((_pinno)>0&&(_pinno)<=HFM_PIN_NUMBER)

#define HF_M_PIN(_no)		HFM_PIN##_no

/*  Default pin configuration (no attribute). */
#define HFPIO_DEFAULT               (0u << 0)
/*  The internal pin pull-up is active. */
#define HFPIO_PULLUP                  (1u << 0)


#define HFPIO_PULLDOWN                  (1u << 1)

/*  Low level interrupt is active */
#define	HFPIO_IT_LOW_LEVEL          	(1u<<4)
/*  High level interrupt is active */
#define HFPIO_IT_HIGH_LEVEL	    		(1u<<5)
/*  Falling edge interrupt is active */
#define HFPIO_IT_FALL_EDGE            	(1u<<6)
/*  Rising edge interrupt is active */
#define HFPIO_IT_RISE_EDGE             	(1u<<7)
/*Interrupt Edge detection is active.*/
#define HFPIO_IT_EDGE			    	(1u<<8)

#define HFPIO_TYPE_Pos                	27
/* PIO Type Mask */
#define HFPIO_TYPE_Msk                	(0x7u << HFPIO_TYPE_Pos)

/*   The pin is an input. */
#define   HFM_IO_TYPE_INPUT       (0x01 << HFPIO_TYPE_Pos)
/*   The pin is an output and has a default level of 0.*/
#define   HFM_IO_OUTPUT_0          (0x02 << HFPIO_TYPE_Pos)
/*   The pin is an output and has a default level of 1.*/
#define   HFM_IO_OUTPUT_1          (0x04 << HFPIO_TYPE_Pos)
#define   HFPIO_DS				  (0x8u << HFPIO_TYPE_Pos)

/*
 *���ݹ���������PIN
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_E_INVAL: fid�Ƿ�,��������Ӧ��PIN�ŷǷ�
 *@HF_FAIL:����ʧ��
 *@HF_E_ACCES:Ҫ���õ�PIN�������ó�ֱ����flags������GND,VCC PIN��,
			  ֻ��������ĽŲ������ó������.
*/
int HSF_API hfgpio_configure_fpin(int fid,int flags);

int HSF_API hfgpio_fconfigure_get(int fid);

int HSF_API hfgpio_fpin_add_feature(int fid,int flags);

int HSF_API hfgpio_fpin_clear_feature(int fid,int flags);

/*
 *���ݹ��������ö�ӦPINΪ����ߵ�ƽ
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_E_INVAL: fid�Ƿ�,��������Ӧ��PIN�ŷǷ�
 *@HF_FAIL:����ʧ��
*/
//int hfgpio_fset_out_high(int fid);
#define hfgpio_fset_out_high(__fid)	hfgpio_configure_fpin(__fid,HFPIO_DEFAULT|HFM_IO_OUTPUT_1)
/*
 *���ݹ��������ö�ӦPINΪ����͵�ƽ
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_E_INVAL: fid�Ƿ�,��������Ӧ��PIN�ŷǷ�
 *@HF_FAIL:����ʧ��
*/

//int hfgpio_fset_out_low(int fid);
#define hfgpio_fset_out_low(__fid)	hfgpio_configure_fpin(__fid,HFPIO_DEFAULT|HFM_IO_OUTPUT_0)


/*
 *����PIN��ID����PIN(�ڲ�ʹ�ã��ⲿ��ʹ��hfgpio_configure_fpin)
 *flags:
		HFPIO_IT_LOW_LEVEL:�͵�ƽ����
		HFPIO_IT_HIGH_LEVEL:�ߵ�ƽ����
		HFPIO_IT_FALL_EDGE:�½��ش���
		HFPIO_IT_RISE_EDGE:�����ش���
		HFPIO_IT_EDGE:���ش���
 *handle:
		�жϴ�����ʱ����ã��������治������ʱ������ʱ������ܶ� 		
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_E_INVAL: pid�Ƿ�
 *@HF_FAIL:����ʧ��
 *@HF_E_ACCES:Ҫ���õ�PIN�������ó��жϣ�PIN�Ų��߱�F_IT����,
*/
int hfgpio_configure_fpin_interrupt(int fid,uint32_t flags,hfgpio_interrupt_func handle,int enable);

/*
 *ʹ���ж�,��ʹ���ж�֮ǰһ��Ҫ�ȵ���hfgpio_configure_fpin_interrupt
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_E_INVAL: fid�Ƿ�,��������Ӧ��PIN�ŷǷ�
 *@HF_FAIL:����ʧ��
*/
int hfgpio_fenable_interrupt(int fid);

/*
 *��ֹ�ж�
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_E_INVAL: fid�Ƿ�,��������Ӧ��PIN�ŷǷ�
 *@HF_FAIL:����ʧ��
*/
int hfgpio_fdisable_interrupt(int fid);

/*
 *��ֹ���е�GPIO �ж�
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_FAIL:����ʧ��
*/
int HSF_API hfgpio_disabel_all_interrupt(void);

/*
 *�������ӦPIN�Ƿ�Ϊ�ߵ�ƽ
 *return value:
 *@>=1:�ߵ�ƽ
 *@<=0:�͵�ƽ,����fid�Ƿ���fid��Ӧ��pin��û�ж�����߲���gpio
*/
int hfgpio_fpin_is_high(int fid);

/*
 *��鹦���뵽PIN��ӳ���Ϸ��У�HFGPIO_F_JTAG_TCK~HFGPIO_F_UART1_CTS��ֻ�����ù̶�
 *��PIN�ţ�����HFM_NOPIN,һ��PIN��ֻ�ܶ�Ӧһ�������롣
 *return value:
 *@1:�̶�������ӳ���PIN�Ŵ���
 *@2:���д��������������Ӧͬһ��PIN��
 *@0:�Ϸ�
*/
int hfgpio_fmap_check(int type);

#ifdef __BUILD_HSF_SDK__

/*
 *����PIN��ID����PIN(�ڲ�ʹ�ã��ⲿ��ʹ��hfgpio_configure_fpin)
 *return value:
 *@HF_SUCCESS:���óɹ�
 *@HF_E_INVAL: pid�Ƿ�
 *@HF_FAIL:����ʧ��
 *@HF_E_ACCES:Ҫ���õ�PIN�������ó�ֱ����flags������GND,VCC PIN��,
			  ,ֻ��������ĽŲ������ó������.
*/
int HSF_IAPI hfgpio_configure_pin(int pid,int flags);

#define  hfgpio_set_out_high(pid)	hfgpio_configure_pin(pid,HFPIO_DEFAULT|HFM_IO_OUTPUT_1)

#define  hfgpio_set_out_low(pid)	hfgpio_configure_pin(pid,HFPIO_DEFAULT|HFM_IO_OUTPUT_0)

int HSF_IAPI hfgpio_get_pid(int pin_no);

void HSF_IAPI hfgpio_init_gpio_pin(void);

int HSF_IAPI hfgpio_pin_is_high(int pid);

#endif


//PWM
int HSF_API  hfgpio_pwm_disable(int fid);

int HSF_API hfgpio_pwm_enable(int fid, int freq, int hrate);

int HSF_API hfgpio_gpio_test_is_running(void);

void HSF_API hfgpio_gpio_test_enable(int enable);

#endif

