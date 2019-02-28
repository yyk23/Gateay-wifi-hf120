#include "hsf.h"

#include "ra_device_state.h"
#include "ra_gpio.h"
#include <coroutine.h>
#include <hm_system.h>
#include <log.h>


 
ra_bool ra_device_state_load(ra_device_state_type_t item)
{
	return device_state_load(item);
}

 
void ra_device_state_store(ra_device_state_type_t item, ra_bool current_state)
{
	device_state_store(item, current_state);
}

 
void ra_register_device_state_changed_callback(
	ra_device_state_changed_callback_t callback)
{
	register_device_state_changed_callback((device_state_changed_callback_t)callback);
}



  inline
int ra_device_state_led_task_install(ra_uint8_t pin, ra_bool reverse)
{
	return device_state_led_task_install(pin, reverse);
}
