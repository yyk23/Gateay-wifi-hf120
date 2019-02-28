#ifndef _HM_SUB_DEVSEND_HANDLE_H_
#define _HM_SUB_DEVSEND_HANDLE_H_

#include "hsf.h"

int hm_sub_devsend_report(hm_48_valid_data_t *valid_data);
int hm_add_sub_dev_report(tid_t *tid, mid_t *mid, uint8 online_status);
int hm_sub_dev_status_report(tid_t *tid, mid_t *mid, uint8 online_status);
#endif