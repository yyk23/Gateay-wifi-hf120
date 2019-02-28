include ../Makefile.mk


CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

OBJECTDIR=objs

CONTIKI_SRC = \
hm_app/src/driver/hm_parameter.c\
hm_app/src/driver/hm_system.c\
hm_app/src/driver/hm_link.c\
hm_app/src/driver/hm_wifi_frame.c\
hm_app/src/utils/RCLutils.c\
hm_app/src/utils/md5sum.c\
hm_app/src/test/hm_test.c\
hm_app/src/main/hm_app_main.c\
hm_app/src/driver/hm_uart.c\
hm_app/src/driver/hm_MD5_sum.c\
hm_app/src/service/hm_uart_handle.c\
hm_app/src/service/hm_connect_cloud_handle.c\
hm_app/src/service/hm_sub_devsend_handle.c\
hm_app/src/service/hm_bind_json_protocol.c\
hm_app/src/common/hm_dev_model.c\
app_main.c





APPCFLAGS = -DSUPPORT_UART_THROUGH\
			-I ./hm_app/include/\
			-I ./hm_app/src/common/\
			-I ./hm_app/src/driver/\
			-I ./hm_app/src/utils/\
			-I ./hm_app/src/test/\
			-I ./hm_app/src/service/\
			-I ./hm_app/src/main/
			

CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o) $(CONTIKI_SRC:.c=.o)}

all:
	@mkdir -p objs
	@mkdir -p objs/hm_app/src
	@mkdir -p objs/hm_app/src/common
	@mkdir -p objs/hm_app/src/driver
	@mkdir -p objs/hm_app/src/utils
	@mkdir -p objs/hm_app/src/test
	@mkdir -p objs/hm_app/src/service
	@mkdir -p objs/hm_app/src/main
	make userapps.a 

userapps.a: $(CONTIKI_OBJS)
	echo "$(AR) $(AROPTS) $@ $^"
	$(AR) $(AROPTS) $@ $^

print:
	echo "CONTIKI_SRC=$(CONTIKI_SRC)"
	echo "CONTIKI_OBJS=$(CONTIKI_OBJS)"

$(OBJECTDIR)/%.o: %.c | $(OBJECTDIR)
	@echo "-----------------make z1 target:$@ $(makefn3)-----------------"
	$(CC) $(CFLAGS) $(APPCFLAGS) -c $< -o $@

clean:
	/bin/rm -f *.o *.elf *.a *.bin *.asm 
	/bin/rm -rf $(OBJECTDIR)

