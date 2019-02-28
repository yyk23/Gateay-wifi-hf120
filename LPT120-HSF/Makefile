### DEfine the CPU directory
include Makefile.mk

MAC_BIN = mc300_mac.hex
CONTIKI_PROJECT=lpt120-main
CONTIKI_SRC=main.c   ###You can add other source code here!!! ###
CONTIKI_OBJ=$(patsubst %.c,%.o,$(CONTIKI_SRC))

TARGET=mc300
CRT0 = boot.s relocate.s
FLASH_LAYOUT=layout_lpt120
ADD_LIB=-L ./thirdpartylib -lairkiss

SDK_DIR   = $(shell pwd)/sdk/2.10

INCLUDE    = -I$(SDK_DIR)/include -I$(SDK_DIR)/include/net -I$(SDK_DIR)/include/bsp -I$(SDK_DIR)/include/net/ieee80211_bss -I$(SDK_DIR)/include/sys -I$(SDK_DIR)/include/net/mac -I$(SDK_DIR)/include/dev
INCLUDE 	 += -I$(SDK_DIR)/include/hsf/include -I$(SDK_DIR)/include/matrixssl
TOOLS_CFLAGS = -I$(SDK_DIR)/include
#HFM_LPT120=0
#HFM_LPB120=1
#HFM_LPT220=2
#HFM_LPB125=3
#HFM_SIP120=4
#HFM_LPT120G=5

__HF_MODULE_ID__ = $(HFM_LPB120)

# -nostdlib -falign-functions=4 -falign-jumps -falign-loops -falign-labels
DBGFLAGS   = -g
OPTFLAGS   = -Os -fomit-frame-pointer

CPPFLAGS  += $(INCLUDE) $(OPTFLAGS) $(DBGFLAGS)
ARCH_FLAGS = -march=armv3m -mno-thumb-interwork 
CFLAGS    += -Wall -Wno-trigraphs -fno-builtin $(CPPFLAGS) $(ARCH_FLAGS) -fdata-sections -ffunction-sections -DMODULE_ID=$(MODULE_ID) -DBOOT_SECTOR_UPDATE=$(BOOT_SECTOR_UPDATE) -DDISABLE_ICOMM_DISCOVER=$(DISABLE_ICOMM_DISCOVER) -Werror=implicit-function-declaration -Werror=maybe-uninitialized
CFLAGS += -DAUTOSTART_ENABLE -D__HF_MODULE_ID__=$(__HF_MODULE_ID__) -D__BUILD_HSF_SDK__

LDFLAGS += --specs=nosys.specs --specs=nano.specs -flto -Wl,-T$(LINKERSCRIPT) 
LDFLAGS += -flto
LDFLAGS += -Xlinker -M -Xlinker -Map=contiki-$(TARGET).map -nostartfiles $(CRT0)
#LDFLAGS += -Wl,--gc-sections

AROPTS = rcf

CLEAN += %.elf $(CONTIKI_PROJECT).elf

#EXAMPLE_NAME=https
EXAMPLE_NAME ?= null

ifeq ($(EXAMPLE_NAME),null)
APPDIR = src
else
APPDIR = example/$(EXAMPLE_NAME)
endif


### Compilation rules

# Don't treat %.elf %.bin as an imtermediate file!
.PRECIOUS: %.elf %.bin

all:
	$(MAKE) CFLAGS="$(CFLAGS)" -C $(APPDIR)
	make lpt120-main.bin

#%.bin:%.elf
lpt120-main.bin: lpt120-main.elf
		@echo "-----------------make 3 target:$@ 1st-dep:$< $(makefn3)-----------------"
		@test -s $(MAC_BIN) || { printf "\n\n$(MAC_BIN)(MAC address embedded inside) does not exist or file size=0 !!!\n";echo "###Please use ./util/gen_mac_bin.exe to generate it once ###\n"; exit 1; }
		@test `stat -c %s $(MAC_BIN)` -eq 8192 || { echo "Please generate $(MAC_BIN),and file size must be 8192(Bytes) "; exit 1; }
		$(OBJCOPY) -O binary $< $@
		@echo -e "\n-----Check bin file size:($@)----->>>"
		test `stat -c %s $@` -lt 640000	###acceptable if < 640KB### 
		@echo "-----Done:bin file size:ok-----<<<"	
		@echo "-----Generate bin for uart-upgrade-----"
		dd skip=$(BOOT_CODE_AND_CONFIG_SIZE) if=$@ of=$(CONTIKI_PROJECT)_os.bin bs=1
		dd skip=0 count=$(BOOT_CODE_SIZE) if=$(CONTIKI_PROJECT).bin of=boot_tmp.bin bs=1
		cat boot_tmp.bin $(MAC_BIN) $(CONTIKI_PROJECT)_os.bin > $(CONTIKI_PROJECT).bin

		@/bin/rm -rf boot_tmp.bin $(CONTIKI_PROJECT)_os.bin dump.log 

		@arm-none-eabi-objdump -h $(CONTIKI_PROJECT).elf > dump.log 
		@chmod +x ./util/*.pl; 
		@./util/chk_overlap.pl
		@./util/rpt_sram_usage.pl dump.log
		@printf "\n\n"; 
		@ls -al *.bin *.elf
		@make layout #show flash layout
		@#make chksum #add checksum to binfile


		@echo "#############################################################"
		@echo "### You can use 'make asm' to generate dis-assembly file!!###"
		@echo "#############################################################"

clean:
	/bin/rm -f *.o *.elf
	/bin/rm -f libmc300.a userapps.a  *.bin *.asm *.map
	cd $(APPDIR) && make clean

%.o: %.c
		@echo "-----------------make z0 target:$@ $(makefn3)-----------------"
		$(CC) $(CFLAGS) $< -c

asm:
	$(OBJDUMP) -D -S lpt120-main.elf > lpt120-main.asm

layout:
		@cd ./util; make CFLAGS="$(TOOLS_CFLAGS)" $(FLASH_LAYOUT); 

chksum:
		@../util/gen_checksum $(MODULE_ID) $(join $(CONTIKI_PROJECT), .bin); 
		
chk:
		@./util/chk_code_size.pl contiki-cabrio.map
sramf:
		@./util/rpt_in_sram_function.pl contiki-cabrio.map

version:
		@svn info | grep "Revision" | awk '{printf "#define SVN_VERSION \".%s\"\n",$$2}' > include/svn_ver.h
		
warn:
		@rm -f compile.log;rm -f warn.log.txt
		@make clean
		@make -j2 > compile.log 2>&1  #due to ubunu's dash shell 
		@printf "\n\n######## Please check warn.log.txt ########\n\n";
		@./util/chk_warning.pl	compile.log > warn.log.txt  

print:
		echo $(CONTIKI_OBJ)

lpt120-main.elf: $(CONTIKI_OBJ) $(LINKERSCRIPT) $(APPDIR)/userapps.a
	# WARNING: if you need math.h. You need to add -lm into this linking stage.Please reference where -lm being added 
	#$(CC) $(LDFLAGS) $(CFLAGS) $(filter-out %.a %.lds,$^) $(filter %.a,$^) -lc $(filter %.a,$^) -lm -o $@ $(ADD_LIB)
	/bin/cp $(SDK_DIR)/libs/libmc300.a . ### use cp to copy library or the linker script will have problem for hierachy issue ###
	/bin/cp $(APPDIR)/userapps.a .
	$(CC) $(LDFLAGS) $(CFLAGS) $(CONTIKI_OBJ) userapps.a libmc300.a libcjsona.a -o lpt120-main.elf -L ./thirdpartylib -lairkiss -lm

