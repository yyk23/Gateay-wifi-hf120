
#include "hsf.h"
#include "powersave.h"

#define UPDATE_COMPLETED 0
#define UPDATE_FAIL 1
#define UPGRADE_DOWNLOAD_SW (24)


PROCESS(update_test_process, "update_test_process");

static char update_test_url[101];
static int update_test_file_length=0;
static int update_test_state=0;

int hf_atcmd_upgrade_sw(pat_session_t s,int argc,char *argv[],char *rsp,int len)
{
	if(argc != 1)
	{
		return -1;
	}

	if(strlen(argv[0]) < 100)
	{
		strcpy(update_test_url, argv[0]);
		process_start(&update_test_process, NULL);
	
		return 0;
	}
	else
		return -4;
}

static int update_test_callback(HF_HTTPC_EVENTDATA *event, void *data)
{
	int recvlen=event->length;
	
	switch(event->eventid)
	{
		case HTTPE_FAIL:
			u_printf("download upgrade software file fail !\n");
			update_test_state = UPDATE_FAIL;
			process_post(&update_test_process,PROCESS_EVENT_CONTINUE,NULL);
			break;
			
		case HTTPE_COMPLETE:
			u_printf("download upgrade software file success !\n");
			update_test_state = UPDATE_COMPLETED;
			process_post(&update_test_process,PROCESS_EVENT_CONTINUE,NULL);
			
			break;
		case HTTPE_RECV_DATA:
			if(hfupdate_write_file(HFUPDATE_SW,update_test_file_length,(uint8_t*)event->data,event->length)<0)
				u_printf("write fail!\r\n");
			update_test_file_length+=recvlen;
			break;
			
		default:
			break;
	}
	
	return 0;
}

PROCESS_THREAD(update_test_process, ev, data)
{
	PROCESS_BEGIN();
	u_printf("Start update test, url[%s].\r\n", update_test_url);
	
	static struct etimer wait_timer;
	static unsigned char update_state=0;

	Switch_Power_mode(0);
	etimer_set(&wait_timer,CLOCK_SECOND*1);
	while(1)
	{
		PROCESS_WAIT_EVENT_UNTIL((ev==PROCESS_EVENT_TIMER)||(ev == PROCESS_EVENT_CONTINUE)||(ev==PROCESS_EVENT_MSG));
		if(update_state == 0)//upload start
		{
			hfupdate_start(HFUPDATE_SW);
			
			update_state++;
			etimer_set(&wait_timer,CLOCK_SECOND*1);
		}
		else if(update_state == 1)//start download
		{
			update_test_file_length=0;
			http_download_file((const char*)update_test_url, update_test_callback, (void*)UPGRADE_DOWNLOAD_SW);

			update_state++;
			etimer_stop(&wait_timer);
		}
		else if(update_state == 2)//upload Installing
		{
			if(update_test_state == UPDATE_COMPLETED)
			{
				if(hfupdate_complete(HFUPDATE_SW, update_test_file_length) == HF_SUCCESS)
				{
					u_printf("update complete successs, restart module!\r\n");
					hfsys_reset();
				}
				else
				{
					u_printf("update complete failed!\r\n");
				}
			}
			else
			{	
				u_printf("update failed.\r\n", update_test_url);
				break;
			}
		}
	}

	u_printf("update test end.\r\n");
	Switch_Power_mode(1);
	update_state = 0;
	
	PROCESS_END();
}
