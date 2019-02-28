#include "hsf.h"
#include "hm_system.h"
#include "hm_uart.h"
#include "hm_uart_handle.h"
#include "hm_connect_cloud_handle.h"
#include "hm_sub_devsend_handle.h"
#include "hm_dev_model.h"
PROCESS(hm_app_main_process, "hm_app_main_process");

int USER_FUNC HM_app_main(void)
{
   /*
       //初始化串口
       //初始化flash存储
       //初始化全局变量，
       //初始化设备模型
       //启动主任务，协调各个任务的进程
       //根据情况，启动不同的任务：
   */
   hm_uart_init();
   hm_product_info_load(&product_info);
   device_detail_print();
   hm_sys_run_info_init();
   hm_dev_mode_init();  //初始化子设备的模型
   process_start(&hm_app_main_process, NULL);

   /*
   hm_48_valid_data_t *pvalid_data = valid_data;
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.dev_name);
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.dev_type);
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.dev_factory);
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.attr_array[0].attr_info.attr_name);
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.attr_array[0].attr_info.datatype);
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.attr_array[1].attr_info.attr_name);
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.attr_array[1].attr_info.datatype);

   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.cmd_info[0].id_type);
   u_printf("HM SUBDEVSEND %s", wt_dev_model_info.cmd_info[1].id_type);
   u_printf("HM SUBDEVSEND %d", wt_dev_model_info.cmd_info[0].cmd_id);
   u_printf("HM SUBDEVSEND %d", wt_dev_model_info.cmd_info[1].cmd_id);
  */

   return 0;
}

PROCESS_THREAD(hm_app_main_process, ev, data)
{
   static struct etimer periodic_timer;
   PROCESS_BEGIN();

   while (1)
   {
      if (hm_sys_run_info.run_info.run_status == RUN_INIT_STATUS)
      {
         u_printf("HM　RUN_INIT_STATUS");
         if (hm_get_wifi_work_mode() == WIFI_AP_MODE)
         {
            //设置STA模式并重启
            hm_set_wifi_work_mode(WIFI_STA_MODE, true);
         }
         while(1)
         {
            etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 1000);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);         
            if (hm_sys_run_info.wifi_link_status == HM_LINK_OK)
            {
               hm_start_connect_cloud();
               hm_sys_run_info.run_info.run_status = RUN_CLOUD_LINK_STATUS;
               break;
            }
         }
      }
      if (hm_sys_run_info.run_info.run_status == RUN_FACTORY_TEST_STATUS)
      {
         u_printf("HM　RUN_FACTORY_TEST_STATUS");
         hm_sys_run_info.run_info.FACTORY_TEST_STATUS = FACTORY_TEST_REQ;
         while(1)
         {
            etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 1000);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
            if (ev == PROCESS_EVENT_TIMER)
            {
               if (hm_sys_run_info.run_info.FACTORY_TEST_STATUS == FACTORY_TEST_REQ)
               {
                  hm_send_start_at_factor_test();
               }
               else if (hm_sys_run_info.run_info.FACTORY_TEST_STATUS == FACTORY_TEST_SUCCESS)
               {
                     //重启
                     HSF_API hfsys_reset();
                     break;
               }              
            }
         }
      }

      if (hm_sys_run_info.run_info.run_status == RUN_WIFI_TEST_STATUS)
      {
         u_printf("HM　RUN_WIFI_TEST_STATUS");
         product_info_set_factory_test(1);
         while (1)
         {
            etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 1000);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
            if (ev == PROCESS_EVENT_TIMER)
            {

               
            }
         }
      }

      if (hm_sys_run_info.run_info.run_status == RUN_WIFI_CONFIG_STATUS)
      {
         u_printf("HM　RUN_WIFI_CONFIG_STATUS");
         product_info_set_factory_test(0);
         ghm_start_link();
         while(1)
         {
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_CONTINUE);
            if (ev == PROCESS_EVENT_CONTINUE)
            {

            }
         }
      }

      //链接服务器
      if (hm_sys_run_info.run_info.run_status == RUN_CLOUD_LINK_STATUS)
      {
         u_printf("HM　RUN_CLOUD_LINK_STATUS");
         
         while(1)
         {
            etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 500);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG || ev == PROCESS_EVENT_CONTINUE || ev == PROCESS_EVENT_TIMER);
            if (ev == PROCESS_EVENT_TIMER)
            {
              
            }
            if (ev == PROCESS_EVENT_CONTINUE)
            {
               if (hm_sys_run_info.run_info.run_status == RUN_NORMAL_STATUS)
               {
                  break;
               }
            }
         }
      }
      if (hm_sys_run_info.run_info.run_status == RUN_NORMAL_STATUS)
      {
         u_printf("HM　RUN_NORMAL_STATUS");
         while(1)
         {
            etimer_set(&periodic_timer, CLOCK_MINI_SECOND * 200);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER || ev == PROCESS_EVENT_CONTINUE);
            if (ev == PROCESS_EVENT_TIMER)
            {
              
            }
         }
      }
   }
   PROCESS_END();
}