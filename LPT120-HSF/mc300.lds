
MEMORY
{
        flash          (rx)   : ORIGIN = 0x3000000, LENGTH = 632K
        ram           (rwx)   : ORIGIN = 0x2000000, LENGTH = 192K
}

/*__stack_end__ = 0x02000000 + 160K - 4;*/
_flash_size_ = LENGTH(flash); 
_ram_size_ = LENGTH(ram); 
__stack_end__ = ORIGIN(ram)+ LENGTH(ram) - 4;
_boot_size = 0x4000;	/*8KB:boot 4KB: wifi-config 4KB:feature_config*/

SECTIONS
{
/*        . = 0x3000000;*/
        . = ORIGIN(flash);

	_sitxt0 = .;
        startup : AT(_sitxt0)
        {
        *(.startup1)
        *(.startup2)
/*		libmc300.a:boot-hiflying.o (.text .rodata .rodata*) */
		. = ALIGN(16);
        } > flash

		_vma_boot_text_beg = ORIGIN(ram) + LENGTH(ram) - 28K;
		_sitxt0a = _sitxt0 + SIZEOF(startup); 


    . = ORIGIN(flash) + _boot_size;
	_sitxt2 = .;

    prog_in_flash _sitxt2 : AT( _sitxt2)
        {
				*(.vars_second_boot)
				libmc300.a:contiki-main.o (.text.startup.main )
/* *(.text*) *(.rodata) *(.rodata*) */
				*(EXCLUDE_FILE (/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_aeabi_uldivmod.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:bpabi.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_dvmd_tls.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divsi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivsi3.o libmc300.a:boot-hiflying.o libmc300.a:uip.o libmc300.a:uip-split.o libmc300.a:timer.o libmc300.a:clock.o libmc300.a:etimer.o libmc300.a:tcpip.o libmc300.a:ieee80211_net.o libmc300.a:ieee80211_rdc.o libmc300.a:packetbuf.o libmc300.a:drv_pktbuf.o libmc300.a:ssvradio.o libmc300.a:ssv6200_rx.o libmc300.a:ssv6200_tx.o libmc300.a:throughput.o libmc300.a:rate_control.o libmc300.a:dbg-printf.o libmc300.a:strformat.o libmc300.a:dbg-puts.o  libmc300.a:debug-uart.o libmc300.a:soft_ap.o libmc300.a:xmem.o libmc300.a:drv_flash.o libmc300.a:flash_api.o libmc300.a:hw_regs_api.o libmc300.a:powersave.o libmc300.a:socket_driver.o libmc300.a:psock.o libmc300.a:sha1ssl.o libmc300.a:aes.o libmc300.a:pstm.o libmc300.a:pstmmonreduce.o libmc300.a:pstmmulcomba.o libmc300.a:pstmsqrcomba.o libmc300.a:asn1.o libmc300.a:x905.o libmc300.a:sha512.o libmc300.a:sha384.o libmc300.a:sha256.o libmc300.a:ssv_lib.o) .text* )
				*(EXCLUDE_FILE (/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_aeabi_uldivmod.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:bpabi.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_dvmd_tls.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divsi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivsi3.o libmc300.a:boot-hiflying.o libmc300.a:uip.o libmc300.a:uip-split.o libmc300.a:timer.o libmc300.a:clock.o libmc300.a:etimer.o libmc300.a:tcpip.o libmc300.a:ieee80211_net.o libmc300.a:ieee80211_rdc.o libmc300.a:packetbuf.o libmc300.a:drv_pktbuf.o libmc300.a:ssvradio.o libmc300.a:ssv6200_rx.o libmc300.a:ssv6200_tx.o libmc300.a:throughput.o libmc300.a:rate_control.o libmc300.a:dbg-printf.o libmc300.a:strformat.o libmc300.a:dbg-puts.o  libmc300.a:debug-uart.o libmc300.a:soft_ap.o libmc300.a:xmem.o libmc300.a:drv_flash.o libmc300.a:flash_api.o libmc300.a:hw_regs_api.o libmc300.a:powersave.o libmc300.a:socket_driver.o libmc300.a:psock.o libmc300.a:sha1ssl.o libmc300.a:aes.o libmc300.a:pstm.o libmc300.a:pstmmonreduce.o libmc300.a:pstmmulcomba.o libmc300.a:pstmsqrcomba.o libmc300.a:asn1.o libmc300.a:x905.o libmc300.a:sha512.o libmc300.a:sha384.o libmc300.a:sha256.o libmc300.a:ssv_lib.o) .text.* )
				*(EXCLUDE_FILE (/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_aeabi_uldivmod.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:bpabi.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_dvmd_tls.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divsi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivsi3.o libmc300.a:boot-hiflying.o libmc300.a:uip.o libmc300.a:uip-split.o libmc300.a:timer.o libmc300.a:clock.o libmc300.a:etimer.o libmc300.a:tcpip.o libmc300.a:ieee80211_net.o libmc300.a:ieee80211_rdc.o libmc300.a:packetbuf.o libmc300.a:drv_pktbuf.o libmc300.a:ssvradio.o libmc300.a:ssv6200_rx.o libmc300.a:ssv6200_tx.o libmc300.a:throughput.o libmc300.a:rate_control.o libmc300.a:dbg-printf.o libmc300.a:strformat.o libmc300.a:dbg-puts.o  libmc300.a:debug-uart.o libmc300.a:soft_ap.o libmc300.a:xmem.o libmc300.a:drv_flash.o libmc300.a:flash_api.o libmc300.a:hw_regs_api.o libmc300.a:powersave.o libmc300.a:socket_driver.o libmc300.a:psock.o libmc300.a:sha1ssl.o libmc300.a:aes.o libmc300.a:pstm.o libmc300.a:pstmmonreduce.o libmc300.a:pstmmulcomba.o libmc300.a:pstmsqrcomba.o libmc300.a:asn1.o libmc300.a:x905.o libmc300.a:sha512.o libmc300.a:sha384.o libmc300.a:sha256.o libmc300.a:ssv_lib.o) .rodata )
				*(EXCLUDE_FILE (/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_aeabi_uldivmod.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:bpabi.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivdi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_dvmd_tls.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divsi3.o /usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivsi3.o libmc300.a:boot-hiflying.o libmc300.a:uip.o libmc300.a:uip-split.o libmc300.a:timer.o libmc300.a:clock.o libmc300.a:etimer.o libmc300.a:tcpip.o libmc300.a:ieee80211_net.o libmc300.a:ieee80211_rdc.o libmc300.a:packetbuf.o libmc300.a:drv_pktbuf.o libmc300.a:ssvradio.o libmc300.a:ssv6200_rx.o libmc300.a:ssv6200_tx.o libmc300.a:throughput.o libmc300.a:rate_control.o libmc300.a:dbg-printf.o libmc300.a:strformat.o libmc300.a:dbg-puts.o  libmc300.a:debug-uart.o libmc300.a:soft_ap.o libmc300.a:xmem.o libmc300.a:drv_flash.o libmc300.a:flash_api.o libmc300.a:hw_regs_api.o libmc300.a:powersave.o libmc300.a:socket_driver.o libmc300.a:psock.o libmc300.a:sha1ssl.o libmc300.a:aes.o libmc300.a:pstm.o libmc300.a:pstmmonreduce.o libmc300.a:pstmmulcomba.o libmc300.a:pstmsqrcomba.o libmc300.a:asn1.o libmc300.a:x905.o libmc300.a:sha512.o libmc300.a:sha384.o libmc300.a:sha256.o libmc300.a:ssv_lib.o) .rodata* )
                *(.glue_7)
                *(.glue_7t)
                *(.ARM.extab)
				*(.gnu.linkonce.armextab.*)
                *(.vfp11_veneer)
                *(.v4_bx)
                *(.iplt)
                *(.rel.dyn)
                *(.rel.iplt)
/*		*(ARM.attributes) */
/* 		*(.ARM.attributes) */
		. = ALIGN(4);
        } > flash

		prog_in_sram : AT( _sitxt3)
        {
            *(.startup_clone)
       		*(keep_in_sram)
			/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_aeabi_uldivmod.o (.text .text* .rodata .rodata*)
			/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divdi3.o (.text .text* .rodata .rodata*)
			/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:bpabi.o (.text .text* .rodata .rodata*)
			/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivdi3.o (.text .text* .rodata .rodata*)
			/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_dvmd_tls.o (.text .text* .rodata .rodata*)
			/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_divsi3.o (.text .text* .rodata .rodata*)
			/usr/bin/../lib/gcc/arm-none-eabi/4.8.4/libgcc.a:_udivsi3.o (.text .text* .rodata .rodata*)
            libmc300.a:uip.o (.text .text* .rodata .rodata*)
            libmc300.a:uip-split.o (.text .text* .rodata .rodata*)
      libmc300.a:psock.o (.text .text* .rodata .rodata*)
 libmc300.a:sha1.o (.text .text* .rodata .rodata*)
			libmc300.a:timer.o (.text .text* .rodata .rodata*)
			libmc300.a:clock.o (.text .text* .rodata .rodata*)
			libmc300.a:etimer.o (.text .text* .rodata .rodata*)
			libmc300.a:tcpip.o (.text .text* .rodata .rodata*)
			libmc300.a:ieee80211_net.o (.text .text* .rodata .rodata*)
			libmc300.a:ieee80211_rdc.o (.text .text* .rodata .rodata*)
			libmc300.a:packetbuf.o (.text .text* .rodata .rodata*)
			libmc300.a:drv_pktbuf.o (.text .text* .rodata .rodata*)
			libmc300.a:ssvradio.o (.text .text* .rodata .rodata*)
			libmc300.a:ssv6200_rx.o (.text .text* .rodata .rodata*)
			libmc300.a:ssv6200_tx.o (.text .text* .rodata .rodata*)
			libmc300.a:throughput.o (.text .text* .rodata .rodata*)
			libmc300.a:rate_control.o (.text .text* .rodata .rodata*)
			libmc300.a:strformat.o (.text .text* .rodata .rodata*)
			libmc300.a:debug-uart.o (.text .text* .rodata .rodata*)
			libmc300.a:soft_ap.o (.text .text* .rodata .rodata*)
			libmc300.a:xmem.o (.text .text* .rodata .rodata*)
			libmc300.a:drv_flash.o (.text .text* .rodata .rodata*)
			libmc300.a:flash_api.o (.text .text* .rodata .rodata*)
			libmc300.a:hw_regs_api.o (.text .text* .rodata .rodata*)
			libmc300.a:powersave.o (.text .text* .rodata .rodata*)
			libmc300.a:socket_driver.o (.text .text* .rodata .rodata*)
			libmc300.a:pstm.o (.text .text* .rodata .rodata*)
			libmc300.a:pstmmonreduce.o (.text .text* .rodata .rodata*)
			libmc300.a:pstmmulcomba.o (.text .text* .rodata .rodata*)
			libmc300.a:pstmsqrcomba.o (.text .text* .rodata .rodata*)
			libmc300.a:asn1.o (.text .text* .rodata .rodata*)
			libmc300.a:x905.o (.text .text* .rodata .rodata*)
 			libmc300.a:sha512.o (.text .text* .rodata .rodata*)
			libmc300.a:sha384.o (.text .text* .rodata .rodata*)
			libmc300.a:sha256.o (.text .text* .rodata .rodata*)
			libmc300.a:ssv_lib.o (.text .text* .rodata .rodata*)
			libmc300.a:aes.o (.text .text* .rodata .rodata*)
			libmc300.a:sha1ssl.o (.text .text* .rodata .rodata*)
   		*(.text.process_thread*)
            . = ALIGN(4);
		} > ram


  _tbeg_dst = ORIGIN(ram); 
	

		_sitxt3 = LOADADDR(prog_in_flash) + SIZEOF(prog_in_flash);


	_tbeg_src = LOADADDR(prog_in_sram);
	_tend_dst = _tbeg_dst + SIZEOF(prog_in_sram);


/*
	if .ARM.exidx removed,there will error message like: "section .ARM.exidx 
	loaded at [000xxxx,0001xxxxx] overlaps section .data loaded at [000xxxxx,000xxxxx]"
*/

	.ARM : 
	{
		*(.ARM.exidx*)	
		_tend_dst = .;
	} > ram

        .data : 
        {
		_dbeg_dst = .;
		*(EXCLUDE_FILE (libmc300.a:boot-hiflying.o) .data)
		*(EXCLUDE_FILE (libmc300.a:boot-hiflying.o) .data*) _dend_dst = .; 
		} >ram 
		_dbeg_src = LOADADDR(.data);


        .bss ALIGN(16):
        {
         	. = ALIGN(16);
                __bss_beg__ = .;
				*(EXCLUDE_FILE (libmc300.a:boot-hiflying.o) .bss )
				*(EXCLUDE_FILE (libmc300.a:boot-hiflying.o) .bss*)
				*(EXCLUDE_FILE (libmc300.a:boot-hiflying.o) COMMON)
        		__bss_end__ = . ;
                . = ALIGN(16);
        } >ram


	_btext_beg_src = LOADADDR(boot_in_sram);
	_btext_beg_dst = _vma_boot_text_beg; 
	_btext_end_dst =  _btext_beg_dst + SIZEOF(boot_in_sram);

		boot_in_sram _vma_boot_text_beg : AT(_sitxt0a)
        {
            . = ALIGN(4);
/*		libmc300.a:boot-hiflying.o (.text .text.* .rodata .rodata*) */
		libmc300.a:boot-hiflying.o (.text )
		libmc300.a:boot-hiflying.o (.text.* )
		libmc300.a:boot-hiflying.o (.rodata )
		libmc300.a:boot-hiflying.o (.rodata* )
            . = ALIGN(4);
		} > ram

		_sitxt1 = _sitxt0a + SIZEOF(boot_in_sram);


/*		_vma_boot_data_beg = 0x2000000 + 160K - 16K;  */ 
		_vma_boot_data_beg = ORIGIN(ram) + LENGTH(ram) - 16K;


        .boot_data _vma_boot_data_beg : AT( _sitxt1) 
        {
			_boot_dbeg_dst = .;
			libmc300.a:boot-hiflying.o (.data .data*)
			. = ALIGN(16);
			_boot_dend_dst = .;
        } >ram
		_boot_dbeg_src = LOADADDR(.boot_data);

		_vma_boot_bss_beg =_vma_boot_data_beg - 16K;   
        .boot_bss :
        {
			__boot_bss_beg__ = .;
			libmc300.a:boot-hiflying.o (.bss .bss* COMMON)
			__boot_bss_end__ = .;
        } >ram


/*
		_boot_dbeg_src = LOADADDR(.boot_data);
		_boot_dbeg_dst =  __boot_bss_end__ - SIZEOF(.boot_data);
*/


        . = ALIGN(4);
        .eh_frame :
        {
                 KEEP (*(.eh_frame))
        } > ram

        /* Align here to ensure that the .bss section occupies space up to
        _end.  Align after .bss to ensure correct alignment even if the
        .bss section disappears because there are no input sections.  */
        . = ALIGN(32 / 8);
        . = ALIGN(32 / 8);

        _end = .;

		 __end__ = . ;


/*TOTAL_STACK_SIZE comes from boot.s*/
/*__heap_start__ = __bss_end__ + 0x4000;*/
__heap_start__ = __bss_end__;
__heap_end__   = ORIGIN(ram)+ LENGTH(ram) - TOTAL_STACK_SIZE - 0x200;
__nvram_start__ = __heap_end__;

/*
	.ARM.attributes : 
	{ 
		*(.ARM.attributes) 
	} > ram
*/

	.ARM.attributes 0 : { *(.ARM.attributes) *(ARM.attributes) }

	
        PROVIDE (end = .);
}

