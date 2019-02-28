#include "hsf.h"
#include "RCLutils.h"
#include "hm_user_config.h"
#define DYNAMIC_STRING_BUFFER_SIZE 256 //byte
#undef ERR_MSG
#undef debug_print
#undef debug_print_str
#undef ASSERT
#define ERR_MSG
#define debug_print
#define debug_print_str
#define ASSERT(exp)    \
	do                 \
	{                  \
		if (exp)       \
		{              \
			goto fail; \
		}              \
	} while (0)

size_t lib_strnlen(const char *s, size_t maxlen)
{
	size_t i;
	for (i = 0; i < maxlen; i++)
	{
		if (s[i] == '\0')
		{
			return i;
		}
	}
	return i;
}

void bitmap_set(size_t *bitmap, size_t num, uint8_t value)
{
	size_t buf = 1 << (num % BITMAP_W);
	if (value == 1)
	{
		bitmap[num / BITMAP_W] |= buf;
	}
	else
	{
		bitmap[num / BITMAP_W] &= ~buf;
	}
}

size_t bitmap_forech(size_t *bitmap, size_t bitmap_length, size_t forech_startnum, uint8_t value)
{

	size_t L = forech_startnum / BITMAP_W;
	for (; L < bitmap_length; L++)
	{
		size_t W;
		for (W = 0; W < BITMAP_W; W++)
		{
			size_t num = L * BITMAP_W + W;
			if (num >= forech_startnum)
			{
				if (((bitmap[L] & (1 << W)) != 0))
				{
					if (value == 1)
						return num;
				}
				else
				{
					if (value == 0)
						return num;
				}
			}
		}
	}
	return 0;
}

unsigned char is_space(char c)
{
	if (c == ' ' || c == '\n' || c == '\t' || c == '\r')
	{
		return 1;
	}
	else
		return 0;
}

unsigned char is_number(char c)
{
	return (c >= '0' && c <= '9') ? 1 : 0;
}

unsigned char str_is_number(char *s, uint32_t strlen)
{
	uint32_t count = 0;
	while (s[count] != '\0' && count < strlen)
	{
		if (is_number(s[count]) == 0)
			return 0;
		count++;
	}
	return 1;
}

unsigned char is_symbol(char c)
{
	// not ( ) ' "  `
	return (c == '!' || (c > 34 && c < 39) || (c > 41 && c < 59) || (c > 59 && c < 96) || (c > 96 && c < 127)) ? 1 : 0;
}

int lib_atoi(char s[])
{
	int i, n, sign;
	for (i = 0; is_space(s[i]); i++)
	{
		;
	}
	sign = (s[i] == '-') ? -1 : 1;
	if (s[i] == '+' || s[i] == '-')
		i++;
	for (n = 0; is_number(s[i]); i++)
		n = 10 * n + (s[i] - '0');
	return sign * n;
}

static uint8_t lib_ctohex(char *onechar)
{
	if (*onechar >= '0' && *onechar <= '9')
	{
		return (uint8_t)(*onechar - '0');
	}
	else if (*onechar >= 'a' && *onechar <= 'f')
	{
		return (uint8_t)(*onechar - 'a') + 10;
	}
	else if (*onechar >= 'A' && *onechar <= 'F')
	{
		return (uint8_t)(*onechar - 'A') + 10;
	}
	else
	{
	//	debug_print("It's not a hex str\n");
		return 0xff;
	}
}

/*
 * exmple : "2B2A" --> 0x2A2B
 */

uint32_t lib_strtohex(uint8_t *des, char *src, uint32_t strlen)
{
	if (strlen % 2 != 0)
	{
	//	debug_print("len % 2 != 0\n");
		return 0;
	}
	uint32_t size = strlen / 2;
	uint32_t count;
	memset(des, 0, size);
	for (count = 0; count < size; count++)
	{
		des[count] = lib_ctohex(src + count * 2 + 1) | (lib_ctohex(src + count * 2) << 4);
	}
	return strlen / 2;
}

void lib_hextostr(char *des, uint8_t *src, uint32_t datalen, bool _case)
{

	uint16_t count;
	for (count = 0; count < datalen; count++)
	{
		if (_case == true)
		{
			sprintf(des + count * 2, "%02X", src[count]);
		}
		else
		{
			sprintf(des + count * 2, "%02x", src[count]);
		}
	}
}

void lib_strtostr(char *des, uint8_t *src, uint32_t datalen, bool _case)
{

	uint16_t count;
	for (count = 0; count < datalen; count++)
	{
		if (_case == true)
		{
			sprintf(des + count, "%c", src[count]);
		}
		else
		{
			sprintf(des + count, "%c", src[count]);
		}
	}
}

static uint8_t isHttpSymbol(char *c)
{
	if ((*c) == ' ' || (*c) == '/' || (*c) == '?' || (*c) == '=' || (*c) == '&')
		return 1;
	else
	{
		return 0;
	}
}

char *get_http_head_value(char *httpdata, const char *target)
{
	uint16_t desszie = (uint16_t)strlen(target);
	uint16_t HttpSymbolPos[HttpSymbolNUM] = {0};
	if (strncmp(httpdata, "GET", 3) == 0)
	{
		uint16_t i = 0;
		uint16_t SymbolPosCount = 0;
		do
		{
			if (isHttpSymbol(httpdata + i) == 1)
			{
				HttpSymbolPos[SymbolPosCount++] = i;
			}
		} while (httpdata[i++] != '\n');

		if (SymbolPosCount == 0)
		{
			return NULL;
		}
		for (i = 0; i < SymbolPosCount - 1; i++)
		{
			if (strncmp(httpdata + HttpSymbolPos[i] + 1, target, desszie) == 0)
			{
				if ((i + 2) < HttpSymbolNUM)
				{
					char *value = (char *)hfmem_malloc(HttpSymbolPos[i + 2] - HttpSymbolPos[i + 1]);
					memcpy(value, httpdata + HttpSymbolPos[i + 1] + 1, HttpSymbolPos[i + 2] - HttpSymbolPos[i + 1] - 1);
					return value;
				}
				return NULL;
			}
		}
		return NULL;
	}
	return NULL;
}

/**
 * Makes a deep copy of this object.
 * @return  des string point (need free)
 */

char *str_clone(const char *src, size_t strlen)
{
	char *des = (char *)hfmem_malloc(strlen + 1);
	if
		unlikely(des == NULL)
		{
			// ERR_MSG("no mem to malloc");
			return NULL;
		}
	memcpy(des, src, strlen);
	des[strlen] = '\0';
	return des;
}

float kalman_filter(const float src_data, float process_niose_q, float measure_noise_r)
{
	float R = measure_noise_r;
	float Q = process_niose_q;

	static double x_last;

	float x_mid = (float)x_last;
	float x_now;

	static float p_last;

	float p_mid;
	float p_now;
	float kg;

	x_mid = (float)x_last;
	p_mid = p_last + Q;
	kg = p_mid / (p_mid + R);
	x_now = x_mid + kg * (src_data - x_mid);

	p_now = (1 - kg) * p_mid;

	p_last = p_now;
	x_last = x_now;

	return x_now;
}

bool get_ip_from_str(uint32_t *ip, const char *str)
{
	uint32_t ip_term_temp[4] = {0};
	char ip_term[4] = {0};
	const char *StartPos = str;
	const char *EndPos = strchr(str, '.');
	uint32_t count = 0;

	/*Check whether the string legally */
	while (str[count] != '\0')
	{
		if (is_number(str[count]) == 1)
		{
			//pass
		}
		else
		{
			if (str[count] == '.')
			{
				//pass
			}
			else
			{
				return false;
			}
		}
		count++;
	}

	for (count = 0; count < 4; count++)
	{
		if
			unlikely(EndPos == NULL) { goto fail; }
		memset(ip_term, 0, 4);
		if
			unlikely(EndPos - StartPos > 4) { goto fail; };

		memcpy(ip_term, StartPos, EndPos - StartPos);
		ip_term_temp[count] = atoi(ip_term);
		StartPos = EndPos + 1;
		EndPos = strchr(StartPos, '.');
		if (count == 2)
			EndPos = str + strlen(str) + 1;
	}
	*ip = ip_term_temp[0] | (ip_term_temp[1] << 8) | (ip_term_temp[2] << 16) | (ip_term_temp[3] << 24);
	goto done;
done:
	return true;
fail:
	return false;
}

bool get_str_from_str(
	char *src,
	const char *match_head,
	const char *match_tail,
	char **des_str,
	char **remainder_str)
{
	char *des_str_t = NULL;
	char *remainder_src_t = NULL;
	char *match_head_pos = match_head[0] == '\0' ? src : (char *)strstr(src, match_head);
	if
		unlikely(match_head_pos == NULL)
		{
			goto fail;
		}

	char *host_head_pos = match_head_pos + strlen(match_head);
	char *host_tail_pos = match_tail[0] == '\0' ? host_head_pos + strlen(host_head_pos) : (char *)strstr(host_head_pos, match_tail);
	if
		unlikely(host_tail_pos == NULL)
		{
			goto fail;
		}
	char *url_tail_pos = src + strlen(src);

	size_t host_size = host_tail_pos - host_head_pos;
	des_str_t = (char *)hfmem_malloc(host_size + 1);
	if
		unlikely(des_str_t == NULL)
		{
			// ERR_MSG("malloc error");
			goto fail;
		}
	memcpy(des_str_t, host_head_pos, host_size);
	des_str_t[host_size] = '\0';
	if
		unlikely(des_str != NULL)
		{
			*des_str = des_str_t;
		}
	else
	{
		free(des_str_t);
	}

	size_t remainder_src_size = url_tail_pos - host_tail_pos;
	remainder_src_t = (char *)hfmem_malloc(remainder_src_size + 1);
	if
		unlikely(remainder_src_t == NULL)
		{
			// ERR_MSG("malloc error");
			goto fail;
		}
	memcpy(remainder_src_t, host_tail_pos, remainder_src_size);
	remainder_src_t[remainder_src_size] = '\0';

	if (remainder_str != NULL)
	{
		*remainder_str = remainder_src_t;
	}
	else
	{
		free(remainder_src_t);
	}
	return true;
fail:
	free(des_str_t);
	free(remainder_src_t);
	return false;
}

void dynamic_param_destory(dynamic_param_t *p)
{
	if (p != NULL)
	{
		switch (p->type)
		{
		case DYNAMIC_PARAM_TYPE_INT:
			break;
		case DYNAMIC_PARAM_TYPE_RAW:
			free(p->u.value_raw.src);
			break;
		case DYNAMIC_PARAM_TYPE_STRING:
			free(p->u.value_str.src);
			break;
		case DYNAMIC_PARAM_TYPE_NULL:
			break;
		default:
			// ERR_MSG("unknow type");
			return;
		}
		free(p);
	}
}

inline void dynamic_param_type_convert(dynamic_param_t *p, dynamic_param_type_t type)
{
	if (p != NULL)
	{
		p->type = type;
	}
}

/*
type:int must be int32_t
*/

dynamic_param_t *dynamic_param_new(dynamic_param_type_t type, const void *src, size_t size)
{
	// char *err_msg = "unknow";
	dynamic_param_t *new_prarm = (dynamic_param_t *)hfmem_malloc(sizeof(dynamic_param_t));
	ASSERT(new_prarm == NULL);

	/*init*/
	memset(new_prarm, 0, sizeof(dynamic_param_t));
	new_prarm->type = type;

	/*store data*/
	switch (type)
	{
	case DYNAMIC_PARAM_TYPE_INT:
		ASSERT(size != sizeof(new_prarm->u.value_int));

		new_prarm->type = DYNAMIC_PARAM_TYPE_INT;
		memcpy(&(new_prarm->u.value_int), src, size);
		break;

	case DYNAMIC_PARAM_TYPE_RAW:
		ASSERT(size == 0);
		new_prarm->u.value_raw.src = (uint8_t *)hfmem_malloc(size);
		new_prarm->u.value_raw.len = size;
		ASSERT(new_prarm->u.value_raw.src == NULL);

		if
			likely(src != NULL)
			{
				memcpy(new_prarm->u.value_raw.src, src, size);
			}
		break;

	case DYNAMIC_PARAM_TYPE_STRING:
		ASSERT(size == 0 || src == NULL);
		size = strlen(src) < size ? strlen(src) : size;
		new_prarm->u.value_str.src = (uint8_t *)hfmem_malloc(size + 1);
		new_prarm->u.value_str.len = size;
		ASSERT(new_prarm->u.value_str.src == NULL);

		memcpy(new_prarm->u.value_raw.src, src, size);
		new_prarm->u.value_str.src[size] = '\0';
		break;

	case DYNAMIC_PARAM_TYPE_NULL:
		new_prarm->type = DYNAMIC_PARAM_TYPE_NULL;
		break;

	default:
		// err_msg = "type error";
		goto fail;
	}
	return new_prarm;
fail:
	dynamic_param_destory(new_prarm);
	return NULL;
}

char *dynstring_append_len(char *des, size_t desStrlen, char *src, size_t srcStrlen)
{
	char *ret = NULL;
	if ((desStrlen + srcStrlen + 1) > DYNAMIC_STRING_BUFFER_SIZE)
	{
		ret = hfmem_malloc(desStrlen + srcStrlen + 1);
		if (ret == NULL)
		{
			return NULL;
		}
		memcpy(ret, des, desStrlen);
		free(des);
	}
	else
	{
		ret = des;
	}

	memcpy(ret + desStrlen, src, srcStrlen);
	ret[desStrlen + srcStrlen] = '\0';
	return ret;
}

char *dynstring_append(char *des, char *src)
{
	size_t desStrlen = strlen(des);
	size_t srcStrlen = strlen(src);
	char *ret = NULL;

	if ((desStrlen + srcStrlen + 1) > DYNAMIC_STRING_BUFFER_SIZE)
	{
		ret = hfmem_malloc(desStrlen + srcStrlen + 1);
		if (ret == NULL)
		{
			return NULL;
		}
		memcpy(ret, des, desStrlen);
		free(des);
	}
	else
	{
		ret = des;
	}

	memcpy(ret + desStrlen, src, srcStrlen);
	ret[desStrlen + srcStrlen] = '\0';
	return ret;
}

char *dynstring_init(void)
{
	char *ret = hfmem_malloc(DYNAMIC_STRING_BUFFER_SIZE);
	if (ret == NULL)
	{
		return NULL;
	}
	ret[0] = '\0';
	return ret;
}

void *lib_memchr(const void *buffer, int ch, int count)
{
	while (count && (*(unsigned char *)buffer != (unsigned char)ch))
	{
		buffer = (unsigned char *)buffer + 1;
		count--;
	}
	return (count ? (void *)buffer : NULL);
}
