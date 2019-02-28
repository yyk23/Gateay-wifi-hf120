
/* hfflash.h
 *
 * Copyright (C) 2013 High-flying Electronics Technology Co.,Ltd.
 *
 * This file is part of HSF.
 *
 */

#ifndef _HF_FLASH_H_H_H_
#define _HF_FLASH_H_H_H_


#define HFFLASH_PAGE_SIZE					(4*1024)
#define HFFLASH_BLOCK_SIZE					(64*1024)


#define FLASH_CAPACITY						(2*1024*1024)
#define MAX_SOFTWARE_CODE_SIZE				(640*1024)
#define MAX_USERCONFIG_SIZE					(16*1024)

#define BOOT_SECTOR_ADDRESS					0x0000000
#define BOOT_SECTOR_SIZE					(8*1024)
#define MP_ADDRESS							0x00002000
#define MP_SIZE								(8*1024)
#define SOFTWARE_CODE_ADDRESS				0x00004000
#define SOFTWARE_CODE_ADDR_END				0x000A4000
#define USERPAGE 							0x000BC000
#define USERPAGE_BACKUP 					0x000BD000
#define F_SETTING_ADDRESS       			0x000BE000 

#define FS_ADDRESS							0x000C0000
#define FS_SIZE								(128*1024)

#define BOOT_CONFIG_ADDRESS					0x000FF000


#define UFLASH_ADDRESS						0x00100000
#define HFUFLASH_SIZE						(120*1024)
#define UFLASH1_ADDRESS						(UFLASH_ADDRESS+HFUFLASH_SIZE)
#define HFUFLASH1_SIZE						(0*1024)
#define USER_BIN_FILE_OFFSET				0x0011E000
#define USER_BIN_FILE_SIZE					4*1024
#define USER_BIN_FILE_BAK_OFFSET			0x0011F000
#define USER_BIN_FILE_BAK_SIZE				(USER_BIN_FILE_SIZE)

#define WEBPAGE_FS_ADDRESS					0x00120000
#define WEBPAGE_FS_SIZE						(192*1024)

#define UPGRADE_ADDRESS_OLD					0x00180000
#define UPGRADE_ADDRESS						0x00160000
#define UPGRADE_ADDRESS_END					0x001FFFFF

int HSF_IAPI flash_init(void);

int HSF_API hfuflash_erase_page(uint32_t addr, int pages);

int HSF_API hfuflash_write(uint32_t addr, uint8_t *data, int len);

int HSF_API hfuflash_read(uint32_t addr, uint8_t *data, int len);

int HSF_IAPI hfflash_copy(uint32_t dstaddr,uint32_t srcaddr,uint32_t len);

int HSF_IAPI flash_erase_blocks(unsigned int addr, int blocks);


void flash_lock(void);
void flash_unlock(void);
int  flash_write(unsigned int addr, uint8_t *data, int len, int no_used);
int  flash_erase_page(unsigned int addr, int pages);
int  flash_read(unsigned int addr, uint8_t *data, int len, int no_used);

int  flash_write_no_lock(unsigned int addr, uint8_t *data, int len, int no_used);

int  flash_erase_page_no_lock(unsigned int addr, int pages);



#endif

