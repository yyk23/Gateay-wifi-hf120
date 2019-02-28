#ifndef __DRV_PWM_H__
#define __DRV_PWM_H__

#include <ssv_config.h>

#define PWM_LEVEL 10
#define NEW_PEW_DRIVER 0

//void drv_pwm_specialfreq(U8 pwmcnt);


typedef enum t_PWM_ID
{
	PWM_A 		= 0,
	PWM_B,
} PWM_ID;

void drv_pwm_init_sip(PWM_ID id);
void drv_pwm_setduty_sip(PWM_ID id, U32 dutycycle);
void drv_pwm_enable_sip(PWM_ID id);
void drv_pwm_deinit_sip(PWM_ID id);
void drv_pwm_disable_sip(PWM_ID id);

void drv_pwm_init();
void drv_pwm_setduty(U32 dutycycle);
void drv_pwm_enable();
void drv_pwm_deinit();
void drv_pwm_disable();
#endif
