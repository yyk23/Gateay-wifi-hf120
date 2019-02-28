include ../../Makefile.mk


CLEAN += %.elf $(CONTIKI_PROJECT).elf

### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

OBJECTDIR=objs

CONTIKI_SRC = \
airlinktest.c

#APPCFLAGS = -DSUPPORT_UART_THROUGH

CONTIKI_OBJS=${addprefix $(OBJECTDIR)/,$(CONTIKI_SRC:.c=.o) $(CONTIKI_SRC:.c=.o)}

all:
	@mkdir -p objs
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

