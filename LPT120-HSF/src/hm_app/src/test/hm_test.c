#include "hsf.h"
#include "hm_parameter.h"
#include "hm_system.h"
#include "hm_test.h"

PROCESS(flash_test_process, "flash_test_process");


PROCESS_THREAD(flash_test_process, ev, data)
{
    PROCESS_BEGIN();
    static struct etimer timer_sleep;
    spi_flash_synchronous_with_protect(ESP_PARAM_START_SEC);
    ra_set_parameter_integer(WIFI_TEST_NAME, FLASH_TEST_NUM);
    ra_set_parameter_string(FACTORY_STRING_TEST_NAME, FLASH_TEST_STRING);

    while (1)
    {
        u_printf("HM FLASH test process \r\n");
        etimer_set(&timer_sleep, 2 * CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_CONTINUE);
        if (ev == PROCESS_EVENT_TIMER)
        {
            uint32_t value=0;
            char string_buf[30];
            ra_get_parameter_integer(WIFI_TEST_NAME, &value);
            ra_get_parameter_string(FACTORY_STRING_TEST_NAME, string_buf, sizeof(string_buf));
            u_printf("HM TEST FLASH NUM is %d \r\n", value);
            u_printf("HM TEST FLASH STRING is %s \r\n", string_buf);
            if (value == FLASH_TEST_NUM && (strcmp(FLASH_TEST_STRING, string_buf)==0))
            {
                u_printf("HM_TEST FLASH PASS ! ! ! \r\n");
            }
            else
            {
                u_printf("HM_TEST FLASH ERROR ! ! ! \r\n");
            } 
                  
           
        }
        break;
    }
    PROCESS_END();
}