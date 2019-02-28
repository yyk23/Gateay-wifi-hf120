#include "hsf.h"
#include "md5sum.h"
#include "ra_types.h"
#include "hm_MD5_sum.h"

int raw2hexlit(char *hexlit, const char *raw, const  size_t raw_size)
{
    const char *p = raw;
    char *dst_p = hexlit;
    size_t i;

    for (i = 0; i != raw_size; i++)
    {
        *dst_p++ = "0123456789abcdef"[(*p >> 4) & 0xF];
        *dst_p++ = "0123456789abcdef"[*p & 0xF];
        p++;
    }

    *dst_p = '\0';

    return 0;
}

int hm_make_authkey(char *dst, const char *randomkey, const char *devtid, const char *devprikey)
{
    MD5_CTX context;
    size_t randomkey_len, devtid_len, devprikey_len;
    size_t merged_len;
    char merged[RANDOMKEY_MAX_LEN +
                DEVTID_MAX_LEN +
                DEVPRIKEY_MAX_LEN + 1];
    char *p = merged;
    unsigned char digest[16];

    if ((randomkey_len = strlen(randomkey)) > RANDOMKEY_MAX_LEN)
    {
        return -1;
    }
    if ((devtid_len = strlen(devtid)) > DEVTID_MAX_LEN)
    {
        return -1;
    }
    if ((devprikey_len = strlen(devprikey)) > DEVPRIKEY_MAX_LEN)
    {
        return -1;
    }
    merged_len = randomkey_len + devtid_len + devprikey_len;

    memcpy(p, randomkey, randomkey_len);
    p += randomkey_len;
    memcpy(p, devtid, devtid_len);
    p += devtid_len;
    memcpy(p, devprikey, devprikey_len);
    p += devprikey_len;
    *p = '\0';

    MD5Init(&context);
    MD5Update(&context, (unsigned char *)merged, (unsigned int)merged_len);
    MD5Final(&context, digest);

    raw2hexlit(dst, digest, 16);
 
    return 0;
}
