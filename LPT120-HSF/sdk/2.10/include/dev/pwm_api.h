#ifndef __PWM_API__H__
#define __PWM_API__H__
#include <drv_pwm.h>
#include <ssv_config.h>

//#if NEW_PEW_DRIVER

/**
 * @brief Enable HW PWM functionality.  
 * @param dutycycle	[IN] the duty cycle of the pulse, 0~10. 0 : always low. 10 : always high.
 * @return the result.  -1 : fail. 0 : success.
 */
int enableHwPWM_sip(PWM_ID id, U32 dutycycle);
/**
 * @brief Disable HW PWM functionality. 
 * @return none.
 */
void disableHwPWM_sip(PWM_ID id);

/**
 * @brief Enable HW PWM functionality.  
 * @param dutycycle	[IN] the duty cycle of the pulse, 0~10. 0 : always low. 10 : always high.
 * @return the result.  -1 : fail. 0 : success.
 */
int enableHwPWM(U32 dutycycle);
/**
 * @brief Disable HW PWM functionality. 
 * @return none.
 */
void disableHwPWM(void);


//int enableHwPWMspecial(U8 cnt);

#endif

