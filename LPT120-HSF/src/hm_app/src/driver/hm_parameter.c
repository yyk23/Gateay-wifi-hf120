
 #include "hsf.h"
 #include "hm_parameter.h"
 #include <hm_system.h>
 #include <RCLutils.h>
 #include "hm_user_config.h"
 #include "c_types.h"

SpiFlashOpResult hm_flash_erase_sector(uint16 sec)
{
	hfuflash_erase_page((sec) * 4096U, 8);
	return SPI_FLASH_RESULT_OK;
}
SpiFlashOpResult hm_flash_read(uint32 addr, uint32 *data, int len)
{
	if (hfuflash_read(addr, (uint8 *)data, len) < 0)
	{
		return SPI_FLASH_RESULT_ERR;
	}
	return SPI_FLASH_RESULT_OK;
}

SpiFlashOpResult hm_flash_write(uint32 addr, uint32 *data, int len)
{
	if (hfuflash_write(addr, (uint8 *)data, len) < 0)
	{
		return SPI_FLASH_RESULT_ERR;
	}
	return SPI_FLASH_RESULT_OK ;
}

int spi_flash_synchronous_with_protect(uint32 src_addr)
{
	uint32_t check_i = 0;
	uint32 check_sum = 0;
	uint32 flash_sum = 0;
	char *des_addr = (char *) hfmem_malloc(KEY_VALUE_BUF_LEN_DEFAULT);
	if (des_addr == NULL)
	{
		return -1;
	}

	hm_flash_read((src_addr + 2) * 4096U, (uint32 *)&flash_sum, 4);
	hm_flash_read((src_addr + 0) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
	for (check_i = 0; check_i < (KEY_VALUE_BUF_LEN_DEFAULT / 4); check_i++)
	{
		check_sum = (check_sum + des_addr[check_i]);
	}
	if (check_sum != flash_sum)
	{
		check_sum = 0;
		hm_flash_read((src_addr + 1) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
		for (check_i = 0; check_i < (KEY_VALUE_BUF_LEN_DEFAULT / 4); check_i++)
		{
			check_sum = (check_sum + des_addr[check_i]);
		}
		if (check_sum != flash_sum)
		{
			u_printf("A&B ERROR!");
			hm_flash_read((src_addr + 0) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
			for (check_i = 0; check_i < (KEY_VALUE_BUF_LEN_DEFAULT / 4); check_i++)
			{
				check_sum = (check_sum + des_addr[check_i]);
			}
			hm_flash_erase_sector(src_addr + 1);
			hm_flash_write((src_addr + 1) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
			hm_flash_erase_sector(src_addr + 2);
			hm_flash_write((src_addr + 2) * 4096U, (uint32 *)&check_sum, 4);
			hfmem_free(des_addr);
			return -2;
		}
		else
		{
			u_printf("A ERROR!");
			hm_flash_read((src_addr + 1) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
			hm_flash_erase_sector(src_addr + 0);
			hm_flash_write((src_addr + 0) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
		}
	}
	else
	{
		check_sum = 0;
		hm_flash_read((src_addr + 1) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
		for (check_i = 0; check_i < (KEY_VALUE_BUF_LEN_DEFAULT / 4); check_i++)
		{
			check_sum = (check_sum + des_addr[check_i]);
		}
		if (check_sum != flash_sum)
		{
			u_printf("B ERROR!");
			hm_flash_read((src_addr + 0) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
			hm_flash_erase_sector(src_addr + 1);
			hm_flash_write((src_addr + 1) * 4096U, (uint32 *)des_addr, KEY_VALUE_BUF_LEN_DEFAULT);
		}
		else
		{
			u_printf("flash done!");
		}
	}

	hfmem_free(des_addr);
	return 0;
}

static void hm_flash_read_with_protect(uint32 src_addr, char *des_addr, uint32 size)
{
	hm_flash_read((src_addr + 0) * 4096U, (uint32 *)des_addr, size);
}

static void hm_flash_write_with_protect(uint32 des_addr, char *src_addr, uint32 size)
{
	uint32 check_i = 0;
	uint32 check_sum = 0;
	for (check_i = 0; check_i < (size / 4); check_i++)
	{
		check_sum = (check_sum + src_addr[check_i]);
	}

	hm_flash_erase_sector(des_addr + 0);
	hm_flash_write((des_addr + 0) * 4096U, (uint32 *)src_addr, size);
	hm_flash_erase_sector(des_addr + 1);
	hm_flash_write((des_addr + 1) * 4096U, (uint32 *)src_addr, size);
	hm_flash_erase_sector(des_addr + 2);
	hm_flash_write((des_addr + 2) * 4096U, (uint32 *)&check_sum, 4);
}

static ra_bool is_forbidden(const char *key)
{
	ra_uint8_t i = 0;
	char keys_array[][10] = {"ctrlKey", "bindKey"};

	for (i = 0; i < sizeof(keys_array) / sizeof(keys_array[0]); i++)
	{
		if ((strncmp(keys_array[i], key, strlen(key)) == 0) && (strlen(keys_array[i]) == strlen(key)))
		{
			u_printf("[%s] is forbidden\n", key);
			return ra_true;
		}
	}

	return ra_false;
}

static void key_value_destory(key_value_t *p)
{
	if (p != NULL)
	{
		if (p->key.str != NULL)
		{
			hfmem_free(p->key.str);
		}

		switch (p->value_type)
		{
		case VALUE_TYPE_INT:
			break;
		case VALUE_TYPE_STR:
			if (p->u.value_str.str != NULL)
			{
				hfmem_free(p->u.value_str.str);
			}
			break;
		default:
			u_printf("unknow type");
			return;
		}
		hfmem_free(p);
	}
}

/*
���б���ƥ���ֵ��
*/
static ra_bool key_search(char *file_str, ra_size_t file_len, key_value_t *key_value, ra_uint32_t *p_key_pos)
{
	ra_uint32_t i;
	for (i = BUF_LEN_LEN; i < file_len; i++)
	{
		/*�Ӽ�ֵ���б����ҵ���ֵ�Ա�ʾ��'#'*/
		if (file_str[i] == KEY_VALUE_SIGN)
		{
			*p_key_pos = i;
			i += KEY_VALUE_SIGN_LEN;
			if ((strncmp(key_value->key.str, &file_str[i], key_value->key.len) == 0) && (file_str[i + key_value->key.len] == SPACE_MARK))
			{
				return ra_true;
			}
			i += 6; //6Ϊkey_value��̸�ʽ������
		}
	}

	return ra_false;
}

static ra_bool get_value_from_key(const char *file_str, ra_size_t file_len, key_value_t *key_value, ra_uint32_t key_offset)
{
	ra_uint32_t value_type_offset = key_offset + KEY_VALUE_SIGN_LEN + key_value->key.len + SPACE_MARK_LEN;
	ra_uint32_t value_offset = value_type_offset + VALUE_TYPE_MARK_LEN + SPACE_MARK_LEN;

	if (file_str[value_type_offset] != key_value->value_type)
	{
		return ra_false;
	}
	if (key_value->value_type == VALUE_TYPE_INT)
	{
		memcpy(&(key_value->u.value_int), &file_str[value_offset], sizeof(int));
	}
	else
	{
		key_value->u.value_str.len = strlen(&file_str[value_offset]);
		key_value->u.value_str.str = (char *)hfmem_malloc(key_value->u.value_str.len + 1);
		memcpy(key_value->u.value_str.str, &file_str[value_offset], key_value->u.value_str.len);
		key_value->u.value_str.str[key_value->u.value_str.len] = '\0';
	}
	return ra_true;
}

static key_value_t *key_value_new(char *file_str, ra_size_t file_len, value_type_t type, const char *key, ra_uint8_t key_size)
{
	key_value_t *key_value = (key_value_t *) hfmem_malloc(sizeof(key_value_t));
	ASSERT(key_value == NULL);
	key_value->key.str = (char *)hfmem_malloc(key_size + 1);
	ASSERT(key_value->key.str == NULL);
	memcpy(key_value->key.str, key, key_size);
	key_value->key.str[key_size] = '\0';
	key_value->key.len = key_size;
	key_value->value_type = type;

	ra_uint32_t key_pos_offset = 0;
	bool key_exist_flag = key_search(file_str, file_len, key_value, &key_pos_offset);
	if (key_exist_flag == ra_false)
	{
		//u_printf("%s isn't exist\n", key_value->key);
		goto fail;
	}
	bool ret = get_value_from_key(file_str, file_len, key_value, key_pos_offset);
	ASSERT(ret == ra_false);

	goto done;
done:
	return key_value;
fail:
	key_value_destory(key_value);
	return NULL;
}
static key_value_t *get_key_value(value_type_t type, const char *key, uint16 start_section)
{
	if (key == NULL)
	{
		return NULL;
	}
	//system_soft_wdt_feed();
	ra_uint8_t param_size = (ra_uint8_t)strlen(key);
	ra_size_t file_len = 0;

	char *file_stream = (char *)hfmem_malloc(KEY_VALUE_BUF_LEN_DEFAULT);
	ASSERT(file_stream == NULL);

	hm_flash_read_with_protect(start_section, file_stream, KEY_VALUE_BUF_LEN_DEFAULT);
	memcpy((void *)&file_len, file_stream, sizeof(ra_size_t));

	if (file_len > KEY_VALUE_BUF_LEN_DEFAULT || file_len < BUF_LEN_LEN)
	{
		file_len = BUF_LEN_LEN;
		goto fail;
	}

	key_value_t *new = key_value_new(file_stream, file_len, type, key, param_size);
	if (new == NULL)
	{
		goto fail;
	}
	goto done;
done:
	__FREE(file_stream);
	return new;
fail:
	__FREE(file_stream);
	return NULL;
}

/*
KEY-VALUE:
format: #KEY TYPE VALUE #KEY TYPE VALUE
*/
static ra_bool replace_key_value(char *file_str, ra_size_t *file_len, key_value_t *key_value, ra_uint32_t key_offset)
{
	ra_size_t total_len = 0;
	ra_uint32_t value_type_offset = key_offset + KEY_VALUE_SIGN_LEN + key_value->key.len + SPACE_MARK_LEN;
	ra_uint32_t value_offset = value_type_offset + VALUE_TYPE_MARK_LEN + SPACE_MARK_LEN;

	if (key_value->value_type != file_str[value_type_offset])
	{
		return ra_false;
	}
	if (key_value->value_type == VALUE_TYPE_INT)
	{
		memset(&file_str[value_type_offset], VALUE_TYPE_INT, VALUE_TYPE_MARK_LEN);
		memcpy(&file_str[value_offset], &(key_value->u.value_int), sizeof(ra_int32_t));
		return ra_true;
	}
	else
	{
		/*�齨Key-Value*/
		ra_size_t key_value_buffer_len = KEY_VALUE_SIGN_LEN + key_value->key.len + 3 * SPACE_MARK_LEN + VALUE_TYPE_MARK_LEN + key_value->u.value_str.len + 1 + 1;
		char *key_value_buffer = (char *)hfmem_malloc(key_value_buffer_len);
		ASSERT(key_value_buffer == NULL);
		/*��ʼ��*/
		ra_size_t sprintf_size = sprintf(key_value_buffer, "#%s c %sc ", key_value->key.str, key_value->u.value_str.str);
		ASSERT(sprintf_size >= key_value_buffer_len);
		ra_uint32_t format_value_type_offset = KEY_VALUE_SIGN_LEN + key_value->key.len + SPACE_MARK_LEN;
		ra_uint32_t format_str_end_offset = format_value_type_offset + VALUE_TYPE_MARK_LEN + SPACE_MARK_LEN + key_value->u.value_str.len;
		memset(&key_value_buffer[format_value_type_offset], VALUE_TYPE_STR, VALUE_TYPE_MARK_LEN);
		memset(&key_value_buffer[format_str_end_offset], 0, 1);

		char *next_key_pos = NULL;
		if ((next_key_pos = lib_memchr(&file_str[value_offset], KEY_VALUE_SIGN, (*file_len) - value_offset)) == NULL) //����"#"
		{
			total_len = key_offset + sprintf_size;
			ASSERT(total_len > KEY_VALUE_BUF_LEN_DEFAULT);

			memcpy(&file_str[key_offset], key_value_buffer, sprintf_size);
			__FREE(key_value_buffer);
			(*file_len) = key_offset + sprintf_size;
			return ra_true;
		}
		else //����"#"
		{
			ra_size_t backup_len = (*file_len) - (next_key_pos - file_str);

			total_len = key_offset + sprintf_size + backup_len;
			ASSERT(total_len > KEY_VALUE_BUF_LEN_DEFAULT);

			char *str_tmp = (char *)hfmem_malloc(backup_len);
			ASSERT(str_tmp == NULL);
			memcpy(str_tmp, next_key_pos, backup_len);
			memcpy(&file_str[key_offset], key_value_buffer, sprintf_size);
			memcpy(&file_str[key_offset + sprintf_size], str_tmp, backup_len);

			__FREE(str_tmp);
			__FREE(key_value_buffer);
			(*file_len) = key_offset + sprintf_size + backup_len;
			return ra_true;
		}
	fail:
		__FREE(key_value_buffer);
		return ra_false;
	}
}
/*
KEY-VALUE:
format: #KEY TYPE VALUE #KEY TYPE VALUE
*/
static ra_bool add_key_value(char *file_str, ra_size_t *file_len, key_value_t *key_value)
{
	/*�齨Key-Value*/
	ra_size_t total_len = *file_len;
	ra_size_t key_value_buffer_len = 0;
	if (key_value->value_type == VALUE_TYPE_INT)
		key_value_buffer_len = KEY_VALUE_SIGN_LEN + key_value->key.len + 3 * SPACE_MARK_LEN + VALUE_TYPE_MARK_LEN + sizeof(int) + 1;
	else
		key_value_buffer_len = KEY_VALUE_SIGN_LEN + key_value->key.len + 3 * SPACE_MARK_LEN + VALUE_TYPE_MARK_LEN + key_value->u.value_str.len + 1 + 1;

	char *key_value_buffer = hfmem_malloc(key_value_buffer_len);
	ASSERT(key_value_buffer == NULL);
	ra_size_t sprintf_size;
	if (key_value->value_type == VALUE_TYPE_INT)
	{
		sprintf_size = sprintf(key_value_buffer, "#%s c 0000 ", key_value->key.str);
		ASSERT(sprintf_size >= key_value_buffer_len);
		ra_uint32_t format_value_type_offset = KEY_VALUE_SIGN_LEN + key_value->key.len + SPACE_MARK_LEN;
		ra_uint32_t format_value_offset = format_value_type_offset + VALUE_TYPE_MARK_LEN + SPACE_MARK_LEN;
		memset(&key_value_buffer[format_value_type_offset], VALUE_TYPE_INT, VALUE_TYPE_MARK_LEN);
		memcpy(&key_value_buffer[format_value_offset], &(key_value->u.value_int), sizeof(int));
	}
	else
	{
		sprintf_size = sprintf(key_value_buffer, "#%s c %sc ", key_value->key.str, key_value->u.value_str.str);
		ASSERT(sprintf_size >= key_value_buffer_len);
		ra_uint32_t format_value_type_offset = KEY_VALUE_SIGN_LEN + key_value->key.len + SPACE_MARK_LEN;
		ra_uint32_t format_str_end_offset = format_value_type_offset + VALUE_TYPE_MARK_LEN + SPACE_MARK_LEN + key_value->u.value_str.len;
		memset(&key_value_buffer[format_value_type_offset], VALUE_TYPE_STR, VALUE_TYPE_MARK_LEN);
		memset(&key_value_buffer[format_str_end_offset], 0, 1);
	}

	total_len += sprintf_size;
	ASSERT(total_len > KEY_VALUE_BUF_LEN_DEFAULT);
	memcpy(&file_str[*file_len], key_value_buffer, sprintf_size); //������strncat

	goto done;
done:
	__FREE(key_value_buffer);
	(*file_len) = (*file_len) + sprintf_size;
	return ra_true;
fail:
	__FREE(key_value_buffer);
	return ra_false;
}

static key_value_t *setup_key_value(const char *key, void *v, value_type_t type)
{
	key_value_t *key_value = (key_value_t *) hfmem_malloc(sizeof(key_value_t));
	ASSERT(key_value == NULL);

	/*setup key*/
	key_value->key.len = strlen(key);
	key_value->key.str = (char *)hfmem_malloc(key_value->key.len + 1);
	ASSERT(key_value->key.str == NULL);
	memcpy(key_value->key.str, key, key_value->key.len);
	key_value->key.str[key_value->key.len] = '\0';

	/*setup value*/
	if (type == VALUE_TYPE_INT)
	{
		key_value->value_type = VALUE_TYPE_INT;
		key_value->u.value_int = *((int *)v);
		//u_printf("key_value->u.value_int = %d\n", key_value->u.value_int);
	}
	else
	{
		key_value->value_type = VALUE_TYPE_STR;
		key_value->u.value_str.len = strlen((char *)v);
		key_value->u.value_str.str = (char *)hfmem_malloc(key_value->u.value_str.len + 1);
		ASSERT(key_value->u.value_str.str == NULL);
		memcpy(key_value->u.value_str.str, v, key_value->u.value_str.len);
		key_value->u.value_str.str[key_value->u.value_str.len] = '\0';
	}
	goto done;
done:
	return key_value;
fail:
	key_value_destory(key_value);
	return NULL;
}

static int delete_parameter(const char *key, uint16 start_section)
{
	ra_uint32_t key_pos_offset = 0;
//	ra_bool flag = ra_false;

	ra_size_t file_len = 0;

	if (key == NULL)
		return RA_ERROR_INVALID_ARGUMENTS;
	//key length limit
	if unlikely(strlen(key) > KEY_LENGTH_MAX)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	char *file_stream = (char *) hfmem_malloc(KEY_VALUE_BUF_LEN_DEFAULT);
	if (file_stream == NULL)
	{
		return RA_ERROR_OUT_OF_MEMORY;
	}

	hm_flash_read_with_protect(start_section, file_stream, KEY_VALUE_BUF_LEN_DEFAULT);
	memcpy((void *)&file_len, file_stream, sizeof(ra_size_t));

	if (file_len > KEY_VALUE_BUF_LEN_DEFAULT || file_len < BUF_LEN_LEN)
	{
		file_len = BUF_LEN_LEN;
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	key_value_t *key_value = setup_key_value(key, " ", VALUE_TYPE_STR);
	if (key_value == NULL)
	{
		return RA_ERROR_OUT_OF_MEMORY;
	}

	ra_bool key_exist_flag = key_search(file_stream, file_len, key_value, &key_pos_offset);
	if (key_exist_flag == ra_false)
	{
		key_value_destory(key_value);
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	ra_uint32_t value_type_offset = key_pos_offset + KEY_VALUE_SIGN_LEN + key_value->key.len + SPACE_MARK_LEN;
	ra_uint32_t value_offset = value_type_offset + VALUE_TYPE_MARK_LEN + SPACE_MARK_LEN;

	char *next_key_pos = NULL;
	if ((next_key_pos = memchr(&file_stream[value_offset], KEY_VALUE_SIGN, file_len - value_offset)) == NULL) //����"#"
	{
		memset(&file_stream[key_pos_offset], 0, (file_len - key_pos_offset));
		file_len = key_pos_offset;
	}
	else //����"#"
	{
		ra_size_t backup_len = file_len - (next_key_pos - file_stream);
		char *str_tmp = (char *)hfmem_malloc(backup_len);
		if (str_tmp == NULL)
		{
			key_value_destory(key_value);
			return RA_ERROR_OUT_OF_MEMORY;
		}
		memcpy(str_tmp, next_key_pos, backup_len);
		memset(&file_stream[key_pos_offset], 0, (file_len - key_pos_offset));
		memcpy(&file_stream[key_pos_offset], str_tmp, backup_len);

		__FREE(str_tmp);
		file_len = key_pos_offset + backup_len;
	}

	/*store parameters*/
	memcpy(file_stream, (void *)&file_len, sizeof(ra_size_t));
	hm_flash_write_with_protect(start_section, file_stream, KEY_VALUE_BUF_LEN_DEFAULT);
	__FREE(file_stream);

	key_value_destory(key_value);
	return RA_SUCCESS;
}

int set_parameter(const char *key, void *value, value_type_t type, uint16 start_section)
{
	ra_uint32_t key_pos_offset = 0;
	ra_bool flag = ra_false;
	ra_size_t file_len = 0;

	if (key == NULL)
		return RA_ERROR_INVALID_ARGUMENTS;
	//key length limit
	if unlikely(strlen(key) > KEY_LENGTH_MAX)
		{
			return RA_ERROR_INVALID_ARGUMENTS;
		}

	//value length limit
	if ((type == VALUE_TYPE_STR) && (strlen(value) > VALUE_LENGTH_MAX))
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	key_value_t *key_value = setup_key_value(key, value, type);
	if (key_value == NULL)
	{
		return RA_ERROR_OUT_OF_MEMORY;
	}

	char *file_stream = (char *) hfmem_malloc(KEY_VALUE_BUF_LEN_DEFAULT);
	if (file_stream == NULL)
	{
		key_value_destory(key_value);
		return RA_ERROR_OUT_OF_MEMORY;
	}

	hm_flash_read_with_protect(start_section, file_stream, KEY_VALUE_BUF_LEN_DEFAULT);
	memcpy((void *)&file_len, file_stream, sizeof(ra_size_t));

	if (file_len > KEY_VALUE_BUF_LEN_DEFAULT || file_len < BUF_LEN_LEN)
	{
		file_len = BUF_LEN_LEN;
	}

	ra_bool key_exist_flag = key_search(file_stream, file_len, key_value, &key_pos_offset);
	if (key_exist_flag == ra_true)
	{
		flag = replace_key_value(file_stream, &file_len, key_value, key_pos_offset);
	}
	else
	{
		flag = add_key_value(file_stream, &file_len, key_value);
	}

	if unlikely(flag == ra_false)
		{
			__FREE(file_stream);
			key_value_destory(key_value);
			return RA_ERROR_INVALID_ARGUMENTS;
		}
	/*store parameters*/
	memcpy(file_stream, (void *)&file_len, sizeof(ra_size_t));
	hm_flash_write_with_protect(start_section, file_stream, KEY_VALUE_BUF_LEN_DEFAULT);
	__FREE(file_stream);

	key_value_destory(key_value);
	return RA_SUCCESS;
}

int ra_set_parameter_integer(const char *key, ra_int32_t value)
{
	if (is_forbidden(key) == ra_true)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	return set_parameter(key, (void *)(&value), VALUE_TYPE_INT, ESP_PARAM_START_SEC);
}

int ra_set_parameter_integer_without_limit(const char *key, ra_int32_t value)
{
	return set_parameter(key, (void *)(&value), VALUE_TYPE_INT, ESP_PARAM_START_SEC);
}

int ra_get_parameter_integer(const char *key, ra_int32_t *value)
{
	key_value_t *key_value = get_key_value(VALUE_TYPE_INT, key, ESP_PARAM_START_SEC);
	if (key_value == NULL)
	{
		return RA_ERROR_INTERNAL;
	}

	*value = key_value->u.value_int;
	key_value_destory(key_value);
	return RA_SUCCESS;
}

int ra_set_parameter_string(const char *key, const char *value)
{
	if (is_forbidden(key) == ra_true)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	return set_parameter(key, (void *)value, VALUE_TYPE_STR, ESP_PARAM_START_SEC);
}

int ra_set_feedback_parameter_string(const char *key, const char *value)
{
	return set_parameter(key, (void *)value, VALUE_TYPE_STR, ESP_FEEDBACKPARAM_START_SEC);
}

int ra_set_parameter_string_without_limit(const char *key, const char *value)
{
	return set_parameter(key, (void *)value, VALUE_TYPE_STR, ESP_PARAM_START_SEC);
}

int ra_get_parameter_string(const char *key, char *buf, ra_size_t buf_len)
{
	ra_uint32_t str_len = 0;
	key_value_t *key_value = get_key_value(VALUE_TYPE_STR, key, ESP_PARAM_START_SEC);
	if (key_value == NULL)
	{
		return RA_ERROR_INTERNAL;
	}

	if (key_value->u.value_str.len > buf_len)
	{
		key_value_destory(key_value);
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	memcpy(buf, key_value->u.value_str.str, key_value->u.value_str.len + 1);
	str_len = key_value->u.value_str.len;
	key_value_destory(key_value);
	return str_len;
}

int ra_get_feedback_parameter_string(const char *key, char *buf, ra_size_t buf_len)
{
	ra_uint32_t str_len = 0;
	key_value_t *key_value = get_key_value(VALUE_TYPE_STR, key, ESP_FEEDBACKPARAM_START_SEC);
	if (key_value == NULL)
	{
		return RA_ERROR_INTERNAL;
	}

	if (key_value->u.value_str.len > buf_len)
	{
		key_value_destory(key_value);
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	memcpy(buf, key_value->u.value_str.str, key_value->u.value_str.len + 1);
	str_len = key_value->u.value_str.len;
	key_value_destory(key_value);
	return str_len;
}

int ra_delete_parameter(const char *key)
{
	if (is_forbidden(key) == ra_true)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	return delete_parameter(key, ESP_PARAM_START_SEC);
}

int ra_delete_parameter_without_limit(const char *key)
{
	return delete_parameter(key, ESP_PARAM_START_SEC);
}
