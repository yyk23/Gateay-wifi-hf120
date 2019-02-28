
#ifndef __HM_MD5_SUM_H__
#define __HM_MD5_SUM_H__
#include "ra_types.h"
#include "c_types.h"

#define RANDOMKEY_MAX_LEN   64
#define DEVTID_MAX_LEN      64
#define DEVPRIKEY_MAX_LEN   64
int hm_make_authkey(char *dst, const char *randomkey, const char *devtid, const char *devprikey);

#endif