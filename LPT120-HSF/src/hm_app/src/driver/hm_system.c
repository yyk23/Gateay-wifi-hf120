#include "hsf.h"
#include "hm_parameter.h"
#include "hm_system.h"
#include "hm_test.h"
#include "RCLutils.h"
#include "bin_version.h"
#include "hm_user_config.h"
static uint8 shm_wifi_get_opmode(void);

product_info_t product_info = {0};
hm_sys_run_info_t hm_sys_run_info = {0};
enum
{
	PARSE_STATE_DIGIT,
	PARSE_STATE_DOT,
};

extern int ra_set_parameter_string_without_limit(const char *key, const char *value);

WIFI_MODE_T hm_get_wifi_work_mode(void)
{
	char *words[3] = {NULL};
	char rsp[64] = {0};
	hfat_send_cmd("AT+WMODE\r\n", sizeof("AT+WMODE\r\n"), rsp, 64);
	if (hfat_get_words(rsp, words, 3) > 0)
	{
		u_printf("\nAT+WMODE=%s\r\n", words[1]);
		if (strcmp(words[1], "STA") == 0)
		{
			return WIFI_STA_MODE;
		}
		else
		{
			return WIFI_AP_MODE;
		}
	}
	return WIFI_STA_MODE;
}

void hm_set_wifi_work_mode(WIFI_MODE_T wifimode, bool treboot)
{
	char rsp[64] = {0};
	hfat_send_cmd("AT+WMODE\r\n", sizeof("AT+WMODE\r\n"), rsp, 64);
	if (wifimode == WIFI_AP_MODE)
	{
		hfat_send_cmd("AT+WMODE=AP\r\n", sizeof("AT+WMODE=AP\r\n"), rsp, 64);
	}
	else
	{
		hfat_send_cmd("AT+WMODE=STA\r\n", sizeof("AT+WMODE=STA\r\n"), rsp, 64);
	}

	if (treboot)
	{
		hfat_send_cmd("AT+Z\r\n", sizeof("AT+Z\r\n"), rsp, 64);
	}
}

void hm_wifi_get_macaddr(uint8_t *mac)
{
	char *words[3] = {NULL};
	char rsp[64] = {0};
	hfat_send_cmd("AT+WSMAC\r\n", sizeof("AT+WSMAC\r\n"), rsp, 64);
	if (hfat_get_words(rsp, words, 3) > 0)
	{
		u_printf("\nAT+WSMAC=%s\r\n", words[1]);
		char *pmac = str_to_hex(words[1], strlen(words[1]));
		memcpy(mac, pmac, 6);
		__FREE(pmac);
	}
}

signed char hm_wifi_station_get_rssi(void)
{
	char *words[3] = {NULL};
	char rsp[64] = {0};
	signed char rssi = 0;
	hfat_send_cmd("AT+WSLQ\r\n", sizeof("AT+WSLQ\r\n"), rsp, 64);
	if (hfat_get_words(rsp, words, 3) > 0)
	{

		u_printf("\nAT+WSLQ=%s\r\n", words[1]);
		char *pmac = str_to_hex(words[1], strlen(words[1]));
		__FREE(pmac);
	}
	return rssi;
}

char *get_tid(void)
{
	uint8_t mac[6];
	hm_wifi_get_macaddr(mac);
	char *tid = (char *)hfmem_malloc(sizeof(DEVNAME) + sizeof(mac) * 2);
	if (tid == NULL)
	{
		return NULL;
	}
	sprintf(tid, "" DEVNAME "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return tid;
}

char *get_mac_string(void)
{
	uint8_t mac[6];
	hm_wifi_get_macaddr(mac);
	char *mac_string = (char *)hfmem_malloc(sizeof(mac) * 2 + 1);
	if (mac_string == NULL)
	{
		return NULL;
	}
	sprintf(mac_string, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return mac_string;
}

//need free
char *station_get_ip_string(void)
{
#define IP_FORMAT "xxx.xxx.xxx.xxx"
	char *words[6] = {NULL};
	char rsp[64] = {0};
	hfat_send_cmd("AT+WANN\r\n", sizeof("AT+WANN\r\n"), rsp, 64);
	if (hfat_get_words(rsp, words, 6) > 0)
	{
		u_printf("\nAT+WANN=%s\r\n", words[2]);
	}
	char *ip = (char *)hfmem_malloc(sizeof(IP_FORMAT) + 1);
	memcpy(ip, words[2], strlen(words[2]) + 1);
	return ip;
}
//need free

char *get_full_bin_version(void)
{
	char *version_str = (char *)hfmem_malloc(sizeof(BIN_VERSION) + 1);
	ASSERT(version_str == NULL);

	memcpy(version_str, BIN_VERSION, sizeof(BIN_VERSION));
	version_str[sizeof(BIN_VERSION)] = '\0';

	goto done;
done:
	return version_str;
fail:
	return NULL;
}

char *str_to_hex(char *src, size_t str_len)
{
	char *dest = NULL;
	size_t dest_len = 0;

	dest = (char *)hfmem_malloc(str_len / 2 + 1);
	if (dest == NULL)
	{
		return NULL;
	}
	dest_len = lib_strtohex(dest, src, str_len);
	if (dest_len != str_len / 2)
	{
		hfmem_free(dest);
		return NULL;
	}
	dest[dest_len] = '\0';

	return dest;
}

unsigned char *parse_version(const char *vstr, const size_t vstrlen, unsigned char *outlen)
{
	unsigned char field_count_out = 0;
	unsigned char *dst = hfmem_malloc(vstrlen);
	int state = PARSE_STATE_DOT;
	int d = 0;
	const char *vstr_p = vstr, *vstr_endp = vstr + vstrlen;
	unsigned char *dst_p = dst;
	unsigned char field_count = 0;
	int dirty = 0;
	while (vstr_p != vstr_endp)
	{
		switch (state)
		{
		case PARSE_STATE_DIGIT:
			if (('0' <= *vstr_p) && (*vstr_p <= '9'))
			{
				d = d * 10 + ((*vstr_p) - '0');
				vstr_p++;
				dirty = 1;
			}
			else if (*vstr_p == '.')
			{
				*dst_p++ = (unsigned char)(d);
				field_count++;
				*(dst + field_count) = 0;
				d = 0;
				vstr_p++;
				state = PARSE_STATE_DOT;
			}
			else
			{
				return NULL;
			}
			break;
		case PARSE_STATE_DOT:
			if (('0' <= *vstr_p) && (*vstr_p <= '9'))
			{
				state = PARSE_STATE_DIGIT;
				dirty = 0;
			}
			else
			{
				return NULL;
			}
			break;
		}
	}
	if (dirty == 1)
	{
		*dst_p++ = (unsigned char)(d);
		field_count++;
		*(dst + field_count) = 0;
	}
	if (state == PARSE_STATE_DOT)
		return NULL;

	*outlen = field_count;
	return dst;
}

char *hex_to_str(char *src, size_t src_len, bool _case)
{
	char *dest = NULL;
	size_t dest_len = 0;

	dest_len = src_len * 2;

	dest = (char *)hfmem_malloc(dest_len + 1);
	if (dest == NULL)
	{
		return NULL;
	}
	lib_hextostr(dest, src, src_len, _case);
	dest[dest_len] = '\0';
	return dest;
}

void hex_to_str_nomalloc(char *dst, char *src, size_t src_len, bool _case)
{
	size_t dest_len = 0;

	dest_len = src_len * 2;

	lib_hextostr(dst, src, src_len, _case);
	dst[dest_len] = '\0';
}

char *str_to_str(char *src, size_t src_len, bool _case)
{
	char *dest = NULL;
	size_t dest_len = 0;

	dest_len = src_len;
	dest = (char *)hfmem_malloc(dest_len + 1);
	if (dest == NULL)
	{
		return NULL;
	}
	lib_strtostr(dest, src, src_len, _case);
	dest[dest_len] = '\0';
	return dest;
}

char *product_info_get_mid(void)
{
	return product_info.mid;
}

int product_info_set_mid(const char *mid)
{
	size_t mid_len = strlen(mid);
	if (product_info.mid != NULL)
	{
		hfmem_free(product_info.mid);
		product_info.mid = NULL;
	}

	product_info.mid = (char *)hfmem_malloc(mid_len + 1);
	if (product_info.mid == NULL)
	{
		return -1;
	}

	memcpy(product_info.mid, mid, mid_len);
	product_info.mid[mid_len] = '\0';
	return 0;
}

char *product_info_get_pin_code(void)
{
	return product_info.pin_code;
}

int product_info_set_pin_code(const char *pin_code)
{
	ASSERT((pin_code == NULL) || (strlen(pin_code) > SERVER_LENGTH_MAX));

	if
		likely((product_info.pin_code == NULL) || (strcmp(product_info.pin_code, pin_code) != 0))
		{
			char value_buffer[SERVER_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(PINCODE_NAME, pin_code) != 0);
			int size = ra_get_parameter_string(PINCODE_NAME, value_buffer, SERVER_LENGTH_MAX);
			ASSERT((size > SERVER_LENGTH_MAX) || (strcmp(pin_code, value_buffer) != 0));
			if
				unlikely(product_info.pin_code)
				{
					hfmem_free(product_info.pin_code);
					product_info.pin_code = NULL;
				}
			product_info.pin_code = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

int product_info_clear_pincode(void)
{
	if (product_info.pin_code)
	{
		ASSERT(ra_delete_parameter(PINCODE_NAME) != 0);
		__FREE(product_info.pin_code);
		product_info.pin_code = NULL;
	}
	return 0;
fail:
	return -1;
}

char *product_info_get_service_hostname(void)
{
	return product_info.service_host;
}

int product_info_set_service_hostname(const char *service_host)
{
	ASSERT((service_host == NULL) || (strlen(service_host) > SERVER_LENGTH_MAX));

	if likely((product_info.service_host == NULL) || (strcmp(product_info.service_host, service_host) != 0))
		{
			char value_buffer[SERVER_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(SERVER_NAME, service_host) != 0);
			int size = ra_get_parameter_string(SERVER_NAME, value_buffer, SERVER_LENGTH_MAX);
			ASSERT((size > SERVER_LENGTH_MAX) || (strcmp(service_host, value_buffer) != 0));
			if unlikely(product_info.service_host)
				{
					hfmem_free(product_info.service_host);
					product_info.service_host = NULL;
				}
			product_info.service_host = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

char *product_info_get_mcu_version(void)
{
	return product_info.mcu_version;
}

int product_info_set_mcu_version(const char *mcu_version)
{
	ASSERT((mcu_version == NULL) || (strlen(mcu_version) > SERVER_LENGTH_MAX));

	if
		likely((product_info.mcu_version == NULL) || (strcmp(product_info.mcu_version, mcu_version) != 0))
		{
			char value_buffer[SERVER_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(MCU_VERSION_NAME, mcu_version) != 0);
			int size = ra_get_parameter_string(MCU_VERSION_NAME, value_buffer, SERVER_LENGTH_MAX);
			ASSERT((size > SERVER_LENGTH_MAX) || (strcmp(mcu_version, value_buffer) != 0));
			if
				unlikely(product_info.mcu_version)
				{
					hfmem_free(product_info.mcu_version);
					product_info.mcu_version = NULL;
				}
			product_info.mcu_version = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

char *product_info_get_config_ssid(void)
{
	return product_info.wifi_sta_info.ssid;
}

int product_info_set_config_ssid(const char *ssid)
{
	ASSERT((ssid == NULL) || (strlen(ssid) > SERVER_LENGTH_MAX));

	if likely((product_info.wifi_sta_info.ssid == NULL) || (strcmp(product_info.wifi_sta_info.ssid, ssid) != 0))
		{
			char value_buffer[SERVER_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(WIFI_SSID_NAME, ssid) != 0);
			int size = ra_get_parameter_string(WIFI_SSID_NAME, value_buffer, SERVER_LENGTH_MAX);
			ASSERT((size > SERVER_LENGTH_MAX) || (strcmp(ssid, value_buffer) != 0));
			if
				unlikely(product_info.wifi_sta_info.ssid)
				{
					hfmem_free(product_info.wifi_sta_info.ssid);
					product_info.wifi_sta_info.ssid = NULL;
				}
			product_info.wifi_sta_info.ssid = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

char *product_info_get_config_password(void)
{
	return product_info.wifi_sta_info.password;
}

int product_info_set_config_password(const char *password)
{
	ASSERT((password == NULL) || (strlen(password) > SERVER_LENGTH_MAX));

	if
		likely((product_info.wifi_sta_info.password == NULL) || (strcmp(product_info.wifi_sta_info.password, password) != 0))
		{
			char value_buffer[SERVER_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(WIFI_PASSWORD_NAME, password) != 0);
			int size = ra_get_parameter_string(WIFI_PASSWORD_NAME, value_buffer, SERVER_LENGTH_MAX);
			ASSERT((size > SERVER_LENGTH_MAX) || (strcmp(password, value_buffer) != 0));
			if
				unlikely(product_info.wifi_sta_info.password)
				{
					hfmem_free(product_info.wifi_sta_info.password);
					product_info.wifi_sta_info.password = NULL;
				}
			product_info.wifi_sta_info.password = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

int32_t product_info_get_service_port(void)
{
	return product_info.service_port;
}

int product_info_set_service_port(int32_t service_port)
{
	if (product_info.service_port != service_port)
	{
		ASSERT(ra_set_parameter_integer(SERVER_PORT_NAME, service_port) != 0);
		product_info.service_port = service_port;
	}

	return 0;
fail:
	return -1;
}

work_mode_t product_info_get_work_mode(void)
{
	return product_info.work_mode;
}

int product_info_set_work_mode(work_mode_t work_mode)
{
	product_info.work_mode = work_mode;
	return 0;
}

token_type_t product_info_get_token_type(void)
{
	return product_info.token_type;
}

int product_info_set_token_type(token_type_t token_type)
{
	product_info.token_type = token_type;
	return 0;
}

encrypt_type_t product_info_get_encrypt_type(void)
{
	return product_info.encrypt_type;
}

int product_info_set_encrypt_type(encrypt_type_t encrypt_type)
{
	ASSERT(encrypt_type != ENCRYPT_TYPE_NONE && encrypt_type != ENCRYPT_TYPE_SSL);
	if (product_info.encrypt_type != encrypt_type)
	{
		ASSERT(ra_set_parameter_integer(ENCRYPT_TYPE, encrypt_type) != 0);
		product_info.encrypt_type = encrypt_type;
	}
	return 0;
fail:
	return -1;
}

char *station_get_current_ssid(void)
{

	char *words[3] = {NULL};
	char rsp[64] = {0};
	hfat_send_cmd("AT+WSSSID\r\n", sizeof("AT+WSSSID\r\n"), rsp, 64);
	if (hfat_get_words(rsp, words, 3) > 0)
	{
		u_printf("\nAT+WSSSID=%s\r\n", words[1]);
	}
	char *ssid = (char *)hfmem_malloc(strlen(words[1]) + 1);
	if (ssid == NULL)
	{
		return NULL;
	}
	memset(ssid, 0, strlen(words[1]) + 1);
	strcpy(ssid, words[1]);

	return ssid;
}

bool wifi_station_config_is_exist(void)
{
	return true;
}

char *product_info_get_prodkey(void)
{
	return product_info.core.prodkey;
}

int product_info_set_prodkey(const char *prodkey)
{
	ASSERT((prodkey == NULL) || (strlen(prodkey) != PRODKEY_LENGTH_MAX));

	if (product_info.core.prodkey && strcmp(product_info.core.prodkey, prodkey) == 0)
	{
		goto done;
	}
	else
	{
		if (product_info_clear_token() != 0)
		{
			return -2;
		}
	}

	char value_buffer[PRODKEY_LENGTH_MAX + 1] = {0};
	ASSERT(ra_set_parameter_string_without_limit(PRODKEY_NAME, prodkey) != 0);
	int size = ra_get_parameter_string(PRODKEY_NAME, value_buffer, PRODKEY_LENGTH_MAX);
	ASSERT((size != PRODKEY_LENGTH_MAX) || (strcmp(prodkey, value_buffer) != 0));

	if (product_info.core.prodkey)
	{
		hfmem_free(product_info.core.prodkey);
		product_info.core.prodkey = NULL;
	}
	product_info.core.prodkey = str_clone(value_buffer, size);

	goto done;

done:
	return 0;
fail:
	return -1;
}

char *product_info_get_devtid(void)
{
	return product_info.core.devtid;
}

int product_info_set_devtid(const char *devtid)
{
	ASSERT((devtid == NULL) || (strlen(devtid) != PRODKEY_LENGTH_MAX));

	if (product_info.core.devtid && strcmp(product_info.core.devtid, devtid) == 0)
	{
		goto done;
	}
	else
	{
		if (product_info_clear_token() != 0)
		{
			return -2;
		}
	}

	char value_buffer[PRODKEY_LENGTH_MAX + 1] = {0};
	ASSERT(ra_set_parameter_string_without_limit(DEVTID_NAME, devtid) != 0);
	int size = ra_get_parameter_string(DEVTID_NAME, value_buffer, PRODKEY_LENGTH_MAX);
	ASSERT((size != PRODKEY_LENGTH_MAX) || (strcmp(devtid, value_buffer) != 0));

	if (product_info.core.devtid)
	{
		hfmem_free(product_info.core.devtid);
		product_info.core.prodkey = NULL;
	}
	product_info.core.devtid = str_clone(value_buffer, size);

	goto done;

done:
	return 0;
fail:
	return -1;
}

char *product_info_get_devprikey(void)
{
	return product_info.core.devprikey;
}

int product_info_set_devprikey(const char *devprikey)
{
	ASSERT((devprikey == NULL) || (strlen(devprikey) != PRODKEY_LENGTH_MAX));

	if (product_info.core.devprikey && strcmp(product_info.core.devprikey, devprikey) == 0)
	{
		goto done;
	}
	else
	{
		if (product_info_clear_token() != 0)
		{
			return -2;
		}
	}

	char value_buffer[PRODKEY_LENGTH_MAX + 1] = {0};
	ASSERT(ra_set_parameter_string_without_limit(DEVPRIKEY_NAME, devprikey) != 0);
	int size = ra_get_parameter_string(DEVPRIKEY_NAME, value_buffer, PRODKEY_LENGTH_MAX);
	ASSERT((size != PRODKEY_LENGTH_MAX) || (strcmp(devprikey, value_buffer) != 0));

	if (product_info.core.devprikey)
	{
		hfmem_free(product_info.core.devprikey);
		product_info.core.devprikey = NULL;
	}
	product_info.core.devprikey = str_clone(value_buffer, size);

	goto done;

done:
	return 0;
fail:
	return -1;
}

char *product_info_get_token(void)
{
	return product_info.core.token;
}

int product_info_set_token(const char *token)
{
	ASSERT((token == NULL) || (strlen(token) > TOKEN_LENGTH_MAX));

	if
		likely((product_info.core.token == NULL) || (strcmp(product_info.core.token, token) != 0))
		{
			char value_buffer[TOKEN_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(TOKEN_NAME, token) != 0);
			int size = ra_get_parameter_string(TOKEN_NAME, value_buffer, TOKEN_LENGTH_MAX);
			ASSERT((size != TOKEN_LENGTH_MAX) || (strcmp(token, value_buffer) != 0));

			if
				unlikely(product_info.core.token)
				{
					hfmem_free(product_info.core.token);
					product_info.core.token = NULL;
				}
			product_info.core.token = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

int product_info_clear_token(void)
{
	if (product_info.core.token)
	{
		ASSERT(ra_delete_parameter(TOKEN_NAME) != 0);
		hfmem_free(product_info.core.token);
		product_info.core.token = NULL;
	}
	return 0;
fail:
	return -1;
}



char *product_info_get_authkey(void)
{
	return product_info.core.authkey;
}

int product_info_set_authkey(const char *authkey)
{
	ASSERT((authkey == NULL) || (strlen(authkey) != AUTHORIZATION_LENGTH_MAX));

	if (product_info.core.authkey && strcmp(product_info.core.authkey, authkey) == 0)
	{
		goto done;
	}
	size_t authkey_len = strlen(authkey);
	if (product_info.core.authkey != NULL)
	{
		hfmem_free(product_info.core.authkey);
		product_info.core.authkey = NULL;
	}

	product_info.core.authkey = (char *)hfmem_malloc(authkey_len + 1);
	if (product_info.core.authkey == NULL)
	{
		goto fail;
	}

	memcpy(product_info.core.authkey, authkey, authkey_len);
	product_info.core.authkey[authkey_len] = '\0';

done :
	return 0;
fail:
	return -1;

}

int product_info_clear_authkey(void)
{
	if (product_info.core.authkey)
	{
		ASSERT(ra_delete_parameter(AUTHKEY_NAME) != 0);
		hfmem_free(product_info.core.authkey);
		product_info.core.authkey = NULL;
	}
	return 0;
fail:
	return -1;
}

char *product_info_get_ctrlkey(void)
{
	return product_info.core.ctrlkey;
}

int product_info_set_ctrlkey(const char *ctrlkey)
{
	ASSERT((ctrlkey == NULL) || (strlen(ctrlkey) > CTRLKEY_LENGTH_MAX));

	if
		likely((product_info.core.ctrlkey == NULL) || (strcmp(product_info.core.ctrlkey, ctrlkey) != 0))
		{
			char value_buffer[CTRLKEY_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(CTRLKEY_NAME, ctrlkey) != 0);
			int size = ra_get_parameter_string(CTRLKEY_NAME, value_buffer, CTRLKEY_LENGTH_MAX);
			ASSERT((size != CTRLKEY_LENGTH_MAX) || (strcmp(ctrlkey, value_buffer) != 0));

			if
				unlikely(product_info.core.ctrlkey)
				{
					hfmem_free(product_info.core.ctrlkey);
					product_info.core.ctrlkey = NULL;
				}
			product_info.core.ctrlkey = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

char *product_info_get_bindkey(void)
{
	return product_info.core.bindkey;
}

int product_info_set_bindkey(const char *bindkey)
{
	ASSERT((bindkey == NULL) || (strlen(bindkey) > BINDKEY_LENGTH_MAX));

	if
		likely((product_info.core.bindkey == NULL) || (strcmp(product_info.core.bindkey, bindkey) != 0))
		{
			char value_buffer[BINDKEY_LENGTH_MAX + 1] = {0};

			ASSERT(ra_set_parameter_string_without_limit(BINDKEY_NAME, bindkey) != 0);
			int size = ra_get_parameter_string(BINDKEY_NAME, value_buffer, BINDKEY_LENGTH_MAX);
			ASSERT((size != BINDKEY_LENGTH_MAX) || (strcmp(bindkey, value_buffer) != 0));

			if
				unlikely(product_info.core.bindkey)
				{
					hfmem_free(product_info.core.bindkey);
					product_info.core.bindkey = NULL;
				}
			product_info.core.bindkey = str_clone(value_buffer, size);
		}

	return 0;
fail:
	return -1;
}

wifi_config_t product_info_get_config_type(void)
{
	return product_info.config_type;
}

int product_info_set_config_type(wifi_config_t config)
{
	int32_t config_tmp = -1;

	ASSERT(config != HEKR_CONFIG && config != AIRKISS_CONFIG);
	if (product_info.config_type != config)
	{
		ASSERT(ra_set_parameter_integer(CONFIG_TYPE, config) != 0);
		ASSERT(ra_get_parameter_integer(CONFIG_TYPE, &config_tmp) != 0);
		ASSERT(config_tmp != config);
		product_info.config_type = config_tmp;
	}

	return 0;
fail:
	return -1;
}

char *product_info_get_airkiss_wechatID(void)
{
	return product_info.airkiss.wechatID;
}

char *product_info_get_airkiss_wechatProductId(void)
{
	return product_info.airkiss.wechatProductId;
}

inline int32_t product_info_get_factory_test(void)
{
	return product_info.factory_test;
}

int product_info_set_factory_test(int32_t flag)
{
	int32_t flag_tmp = -1;
	if (product_info.factory_test != flag)
	{
		ASSERT(ra_set_parameter_integer(WIFI_TEST_NAME, flag) != 0);
		ASSERT(ra_get_parameter_integer(WIFI_TEST_NAME, &flag_tmp) != 0);
		ASSERT(flag_tmp != flag);
		product_info.factory_test = flag_tmp;
	}

	return 0;
fail:
	return -1;
}

static int32_t get_parameter_integer(const char *key)
{
	int32_t value;

	int ret = ra_get_parameter_integer(key, &value);
	if (ret != 0)
	{
		return -1;
	}

	return value;
}

static char *get_parameter_string(const char *key)
{
	char *value = NULL;
	char value_buffer[KEY_LENGTH_MAX + 1];

	int size = ra_get_parameter_string(key, value_buffer, KEY_LENGTH_MAX);
	if (size <= 0)
	{
		return NULL;
	}

	value = (char *)hfmem_malloc(size + 1);
	if (value == NULL)
	{
		return NULL;
	}
	memcpy(value, value_buffer, size);
	value[size] = '\0';

	return value;
}

static char *get_feedback_parameter_string(const char *key)
{
	char *value = NULL;
	char value_buffer[KEY_LENGTH_MAX + 1];

	int size = ra_get_feedback_parameter_string(key, value_buffer, KEY_LENGTH_MAX);
	if (size <= 0)
	{
		return NULL;
	}

	value = (char *)hfmem_malloc(size + 1);
	if (value == NULL)
	{
		return NULL;
	}
	memcpy(value, value_buffer, size);
	value[size] = '\0';

	return value;
}

void hm_product_info_load(product_info_t *info)
{
	//上电同步flash存储区域
	spi_flash_synchronous_with_protect(ESP_PARAM_START_SEC);

	info->factory_test = get_parameter_integer(WIFI_TEST_NAME);

	info->config_type = get_parameter_integer(CONFIG_TYPE);
	if (info->config_type == -1)
		info->config_type = HEKR_CONFIG;

	info->core.authkey = get_parameter_string(AUTHKEY_NAME);
	info->core.prodkey = get_parameter_string(PRODKEY_NAME);
	info->core.devtid = get_parameter_string(DEVTID_NAME);
	info->core.devprikey = get_parameter_string(DEVPRIKEY_NAME);
	info->pin_code = get_parameter_string(PINCODE_NAME);

	info->mcu_version = get_parameter_string(MCU_VERSION_NAME);
	info->core.ctrlkey = get_parameter_string(CTRLKEY_NAME);
	info->core.bindkey = get_parameter_string(BINDKEY_NAME);
	info->core.token = get_parameter_string(TOKEN_NAME);
	info->service_host = get_parameter_string(SERVER_NAME);
	info->service_port = get_parameter_integer(SERVER_PORT_NAME);
	info->encrypt_type = get_parameter_integer(ENCRYPT_TYPE);
	info->airkiss.wechatID = get_parameter_string(AIRKISS_WECHAT_ID);
	info->airkiss.wechatProductId = get_parameter_string(AIRKISS_WECHAT_PRODUCT_ID);

	info->wifi_sta_info.ssid = get_parameter_string(WIFI_SSID_NAME);
	info->wifi_sta_info.password = get_parameter_string(WIFI_PASSWORD_NAME);

	info->core.feedbackauthkey = get_feedback_parameter_string(AUTHKEY_NAME);
	info->core.feedbackprodkey = get_feedback_parameter_string(PRODKEY_NAME);

	if (info->core.authkey == NULL)
	{
		if (info->core.token)
		{
			product_info_set_authkey(info->core.token);
		}
	}
	if (info->core.feedbackauthkey && info->core.feedbackprodkey)
	{
		u_printf("has feedback data");
		info->feedback_exist = true;
		if (info->core.authkey == NULL)
			info->core.authkey = info->core.feedbackauthkey;
		if (info->core.prodkey == NULL)
			info->core.prodkey = info->core.feedbackprodkey;
	}
	else
	{
		u_printf("no feedback data");
		info->feedback_exist = false;
	}
}

void hm_restory_factory_data(product_info_t *info)
{
	ra_delete_parameter(PRODKEY_NAME);
	ra_delete_parameter(DEVTID_NAME);
	ra_delete_parameter(DEVPRIKEY_NAME);
	__FREE(info->core.prodkey);
	__FREE(info->core.devtid );
	__FREE(info->core.devprikey);
	info->core.prodkey = NULL;
	info->core.devtid = NULL;
	info->core.devprikey = NULL;
}

void hm_restory_app_data(product_info_t *info)
{
	ra_delete_parameter(WIFI_TEST_NAME);
//	ra_delete_parameter(CTRLKEY_NAME);
	//ra_delete_parameter(BINDKEY_NAME);
	//ra_delete_parameter(AUTHKEY_NAME);

	info->factory_test = 0;
//	info->core.ctrlkey = NULL;
//	info->core.bindkey = NULL;
	//info->core.authkey = NULL;
}

void hm_restory_all_data(product_info_t *info)
{
	hm_restory_factory_data(info);
	hm_restory_app_data(info);
}

static char detail[512];

void device_detail_print(void)
{

#define __DETAIL_FORMAT "{\"devTid\" : \"%s\", "             \
						"\"prodkey\" : \"%s\", "             \
						"\"devprikey\" : \"%s\", "           \
						"\"pincode\" : \"%s\", "           \
						"\"binVer\" : \"" BIN_VERSION "\", " \
						"\"SDKVer\" : \"" SDK_VERSION "\", " \
						"\"rebootreason\" : %d, "            \
						"\"testflag\" : %d, "                \
						"\"host\" : \"%s\", "                \
						"\"port\" : %d, "                    \
						"\"encrypt_type\" : %d, "            \
						"\"ctrlKey\" : \"%s\", "             \
						"\"bindKey\" : \"%s\", "             \
						"\"token\" : \"%s\", "               \
						"\"wifi config\" : %d, "             \
						"}"

	size_t sprintf_len = sprintf(detail, __DETAIL_FORMAT,
								 product_info.core.devtid,
								 product_info.core.prodkey,
								 product_info.core.devprikey,
								 product_info.pin_code,
								 hfsys_get_reset_reason(),
								 product_info.factory_test,
								 product_info.service_host,
								 product_info.service_port,
								 product_info.encrypt_type,
								 product_info.core.ctrlkey,
								 product_info.core.bindkey,
								 product_info.core.token,
								 product_info.config_type);

	char tdata[255];

	if (sprintf_len > 255)
	{
		memcpy(tdata, detail, 250);
		tdata[250] = '\0';
		u_printf("HM information\r\n");
		u_printf("[%s", tdata);
		u_printf("%s] \r\n", &(detail[250]));
	}
	else
	{
		u_printf("HM information\r\n");
		u_printf("[%s] \r\n", detail);
	}
}

/***********************************************
 * 
 * HM define
 * 
 * ********************************************/

uint8 hm_get_wifi_mode(void)
{
	uint8 mode = 0x01;
	uint8_t wifi_mod = hm_sys_run_info.run_info.run_status;
	switch (wifi_mod)
	{
	case RUN_INIT_STATUS:
	case RUN_FACTORY_TEST_STATUS:
	case RUN_CLOUD_LINK_STATUS:
	case RUN_NORMAL_STATUS:
	case RUN_WIFI_TEST_STATUS:
		mode = 0x01;
		break;
	case RUN_WIFI_CONFIG_STATUS:
		mode = 0x02;
		break;
	default:
		mode = 0x01;
		break;
	}
	return mode;
}

uint8 hm_get_wifi_status(void)
{
	if (hm_sys_run_info.wifi_link_status == HM_LINK_OK)
	{
		return 0x01;
	}
	return 0x02;
}
uint8 hm_get_cloud_connect_status(void)
{
	if (hm_sys_run_info.cloud_link_status == CLOUD_LINK_DEV)
	{
		return 0x01;
	}
	return 0x02;
}
uint8 hm_get_signal_qual(void)
{
	return 0x04;
}
uint8 hm_get_device_test_status(void)
{
	return 0x01;
}

int hm_sys_run_info_init(void)
{
	memset(&hm_sys_run_info, 0, sizeof(hm_sys_run_info_t));

	hm_sys_run_info.run_info.run_status = RUN_INIT_STATUS;
	//如果没有进行过厂测，则直接进入到厂测
	if (product_info_get_devtid() == NULL)
	{
		hm_sys_run_info.run_info.run_status = RUN_FACTORY_TEST_STATUS;
	}

	
	if (product_info_get_factory_test() == WIFI_TEST_FLAG)
	{
		//如果进入到wifi厂测模式
		hm_sys_run_info.run_info.run_status = RUN_WIFI_TEST_STATUS;
	}
	else if (product_info_get_factory_test() == WIFI_CONFIG_FLAG)
	{
		//如果进入到意见配网模式
		hm_sys_run_info.run_info.run_status = RUN_WIFI_CONFIG_STATUS;
	}
}