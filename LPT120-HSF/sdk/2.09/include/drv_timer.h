#ifndef _DRV_TIMER_H_
#define _DRV_TIMER_H_

#include "ssv_regs.h"
#include <irq.h>


#define US_TIMER0                       (TU0_US_REG_BASE + 0)
#define US_TIMER1                       (TU0_US_REG_BASE + 0x10)
#define US_TIMER2                       (TU0_US_REG_BASE + 0x20)  //system used
#define US_TIMER3                       (TU0_US_REG_BASE + 0x30)
#define MS_TIMER0                       (TM0_MS_REG_BASE + 0)
#define MS_TIMER1                       (TM0_MS_REG_BASE + 0x10)
#define MS_TIMER2                       (TM0_MS_REG_BASE + 0x20)
#define MS_TIMER3                       (TM0_MS_REG_BASE + 0x30)

#define MAX_HW_TIMER                    8



/**
 * Hardware Timer Operation Mode
 *
 * @ HTMR_ONESHOT: Timer counts down to 0 and then stop.
 * @ HTMR_PERIODIC: Timer counts down to 0 and the reload.
 */
enum hwtmr_op_mode {
    HTMR_ONESHOT = 0,
    HTMR_PERIODIC = 1,
};


#define HTMR_COUNT_VAL(x)               ((x)&0xFFFF)
#define HTMR_OP_MODE(x)                 (((x)&0x01)<<16)
#define HTMR_GET_IRQ(x)                 (((HW_TIMER*)(x) - REG_TIMER_US_0)+ \
                                         IRQ_US_TIMER0)

void hwtmr_init(void);
int hwtmr_stop(HW_TIMER *tmr);
int hwtmr_start(HW_TIMER *tmr, unsigned int count, irq_handler tmr_handle,
                   void *m_data, enum hwtmr_op_mode mode);

#endif /* _DRV_TIMER_H_ */

