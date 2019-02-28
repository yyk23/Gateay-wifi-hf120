
#ifndef _HF_TYPES_H_H_
#define _HF_TYPES_H_H_

typedef char bool;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

typedef unsigned int size_t;

#define __packed __attribute__((packed))

#define LOCAL static

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

/* probably should not put STATUS here */
typedef enum
{
    OK = 0,
    FAIL,
    PENDING,
    BUSY,
    CANCEL,
} STATUS;

#define BIT(nr) (1UL << (nr))

#define REG_SET_BIT(_r, _b) (*(volatile uint32_t *)(_r) |= (_b))
#define REG_CLR_BIT(_r, _b) (*(volatile uint32_t *)(_r) &= ~(_b))

#define DMEM_ATTR __attribute__((section(".bss")))
#define SHMEM_ATTR

#ifdef ICACHE_FLASH
#define FUN_ATTRIBUTE __attribute__((section(".irom0.text")))
#define ICACHE_RODATA_ATTR __attribute__((section(".irom.text")))
#else
#define FUN_ATTRIBUTE
#define ICACHE_RODATA_ATTR
#endif /* ICACHE_FLASH */

#ifndef __cplusplus
typedef unsigned char bool;
#define BOOL bool
#define true(1)
#define false(0)
#define TRUE true
#define FALSE false

#endif /* !__cplusplus */

#endif

