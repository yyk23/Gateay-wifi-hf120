CROSS_COMPILE ?= arm-none-eabi-
CROSS ?= $(CROSS_COMPILE)

### Compiler definitions
CC       = $(CROSS)gcc
LD       = $(CROSS)gcc
AS       = $(CROSS)as
NM       = $(CROSS)nm
OBJCOPY  = $(CROSS)objcopy
STRIP    = $(CROSS)strip
OBJDUMP  = $(CROSS)objdump

AROPTS = rcf



######################
#	MODULE SWITCH    #
######################
BOOT_SECTOR_UPDATE=0
DISABLE_ICOMM_DISCOVER=0
MODULE_HIFLYING=5
MODULE_ID=5

######################
#	FUNCTION SWITCH  #
######################
FUNC_OTA_ON=0

#####################
#	VARIABLES  		#
#####################


ifeq ($(MODULE_ID),$(MODULE_HIFLYING))
LINKERSCRIPT=mc300.lds
BOOT_CODE_AND_CONFIG_SIZE=16384
BOOT_CODE_SIZE=8192
#CONFIG_BIN=
HFM_LPT120=0
HFM_LPB120=1
HFM_LPT220=2
HFM_LPB125=3
HFM_SIP120=4
HFM_LPT120G=5
endif

#HFM_LPT120=0
#HFM_LPB120=1
#HFM_LPT220=2


