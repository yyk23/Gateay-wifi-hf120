#ifndef _WDOG_API_H
#define _WDOG_API_H

#include <ssv_types.h>

void api_wdog_reboot();
void api_wdog_period_5000ms();
void api_wdog_period_10000ms();
void api_wdog_period_cutommed(S32 wtimer_in_ms);
void api_wdog_stop();
void api_softwdog_enable(int enable);

#endif  //_WDOG_API_H