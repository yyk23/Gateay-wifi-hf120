LPT120-MAIN = $(shell pwd)

mac:
	gcc $(CFLAGS) gen_mac_bin.c -DMODULE_ID=2 -o gen_mac_bin.exe
	./gen_mac_bin.exe 11-22-33-44-55-66

#config:
#	gcc $(CFLAGS) gen_config_bin.c -DMODULE_ID=2 -o gen_config_bin.exe
#	./gen_config_bin.exe

flash_layout_boot_4k:
	echo CFLAGS = $(CFLAGS)
	@gcc $(CFLAGS) print_layout.c -DMODULE_ID=2 -o print_layout.exe
	@./print_layout.exe

flash_layout_boot_8k:
	echo CFLAGS = $(CFLAGS)
	@gcc $(CFLAGS) print_layout.c -DMODULE_ID=5 -o print_layout.exe
	@./print_layout.exe

	
layout_lpt120:
	echo CFLAGS = $(CFLAGS)
	@gcc $(CFLAGS) print_layout.c -DMODULE_ID=5 -o print_layout.exe
	@./print_layout.exe	$(LPT120-MAIN)/../lpt120-main
