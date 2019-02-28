#ifndef __HM_BIND_JSON_PROTOCOL_H__
#define __HM_BIND_JSON_PROTOCOL_H__

#include "hsf.h"
#include <hm_system.h>


#define APPTID_LENGTH_MAX 64

#define ACTION_CODE_SUCCESS					200
#define ACTION_CODE_IDLE					201
#define ACTION_CODE_ERROR_PARSE_ERROR		2000102
#define ACTION_CODE_ERROR_ACTION_UNKNOWN	2000103
#define ACTION_CODE_ERROR_UART_TIMEOUT		2000105
#define ACTION_CODE_ERROR_FORMAT			2000106
#define ACTION_CODE_ERROR_DEVTID			2000107
#define ACTION_CODE_ERROR_CTRLKEY			2000109
#define ACTION_CODE_ERROR_EXECUTION 		2000114
#define ACTION_CODE_ERROR_BIN_TYPE			2000115
#define ACTION_CODE_ERROR_UPDATE_STARTED 	2000116
#define ACTION_CODE_ERROR_FRAME_INVALID 	2000117
#define ACTION_CODE_ERROR_INTERNAL_ERROR 	2000118
#define ACTION_CODE_ERROR_NO_LAN_USER 		2000119
#define ACTION_CODE_ERROR_VALUE_INVALID		2000120
#define ACTION_CODE_ERROR_CMDID_INVALID		2000123
#define ACTION_CODE_ERROR_LAN_USER_FULL		2000122
#define ACTION_CODE_ERROR_MCUUPGRADE_BUSY	1400038


#define JSON_PROTOCOL_FIELD_MSGID		"msgId"
#define JSON_PROTOCOL_FIELD_ACTION		"action"
#define JSON_PROTOCOL_FIELD_PARAMS		"params"
#define JSON_PROTOCOL_FIELD_FEATURE		"features"
#define JSON_PROTOCOL_FIELD_CODE		"code"
#define JSON_PROTOCOL_FIELD_DESC		"desc"



#define JSON_PROTOCOL_FIELD_DEVTID		"devTid"
#define JSON_PROTOCOL_FIELD_SUBDEVTID   "subDevTid"
#define JSON_PROTOCOL_FIELD_APPTID		"appTid"
#define JSON_PROTOCOL_FIELD_CMDID		"cmdId"
#define JSON_PROTOCOL_FIELD_DATA		"data"
     


#define JSON_ACTION_RESP_LENGTH_MAX 	20
#define RESP 							"Resp"
#define JSON_PROTOCOL_ACTION_RESP_APPSEND "appSendResp"
#define ACTION_RESP_UNKNOWN_NAME         "errorResp"
#define JSON_PROTOCOL_ACTION_DEVSEND	"devSend"
#define JSON_PROTOCOL_ACTION_ADDSUBDEV          "addSubDev"
#define JSON_PROTOCOL_ACTION_REPORTSUBDEVINFO   "reportSubDevInfo"
#define MSG_JSON_LENGTH_MAX 512

typedef void (*jad_bind_callback_t)(cJSON *whole);

typedef struct 
{
	const char *name;
	jad_bind_callback_t callback;
} jad_bind_item_t;



char *get_code_desc(uint32_t code);
void action_resp_handle(cJSON *msgid, cJSON *action, cJSON *params, uint32_t code);
int bind_json_protocol_handle(char *json_str, unsigned short len);

int hm_cloud_json_handle(char *buf, unsigned short len);

#endif