#include "hsf.h"


PROCESS(main_process, "main process");

/*---------------------------------------------------------------------------*/
AUTOSTART_PROCESSES(&main_process);

/*---------------------------------------------------------------------------*/

extern 	int  app_main(void);


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(main_process, ev, data)
{
	static struct etimer wait_timer;

	PROCESS_BEGIN();

	etimer_set(&wait_timer,CLOCK_MINI_SECOND*10);
	PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
	
	app_main();
	
    PROCESS_END();
}


