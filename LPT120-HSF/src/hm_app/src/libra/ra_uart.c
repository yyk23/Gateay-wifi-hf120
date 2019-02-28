#include "hsf.h"
#include "ra_uart.h"
#include <hm_system.h>
#include <log.h>
#include <glist.h>

#define DEFAULT_SEND_TIMING_INTERVAL	100	//ms
static GList *uart0_send_data_list, *uart1_send_data_list;
os_timer_t g_uart0_timer, g_uart1_timer;
ra_uint32_t uart0_send_interval = DEFAULT_SEND_TIMING_INTERVAL;
ra_uint32_t uart1_send_interval = DEFAULT_SEND_TIMING_INTERVAL;

static void uart0_timing_send_cb(void *arg);
static void uart1_timing_send_cb(void *arg);

  static
ra_bool uart_id_is_valid(ra_uart_id_t uart_id)
{
	switch (uart_id)
	{
	case RA_UART0:
	case RA_UART1:
		return ra_true;
		break;
	default:
		return ra_false;
		break;
	}
}

  static
ra_bool uart_baudrate_is_valid(ra_uart_rate_t rate)
{
	switch (rate)
	{
	case RA_UART_RATE_4800:
	case RA_UART_RATE_9600:
	case RA_UART_RATE_19200:
	case RA_UART_RATE_38400:
	case RA_UART_RATE_57600:
	case RA_UART_RATE_74880:
	case RA_UART_RATE_115200:
	case RA_UART_RATE_230400:
	case RA_UART_RATE_460800:
	case RA_UART_RATE_921600:
	case RA_UART_RATE_UNCHANGED:
		return ra_true;
		break;

	default:
		return ra_false;
		break;
	}
}

  static
ra_bool uart_data_bits_is_valid(ra_uart_data_bits_t data_bits)
{
	switch (data_bits)
	{
	case RA_UART_DATA_BITS_5:
	case RA_UART_DATA_BITS_6:
	case RA_UART_DATA_BITS_7:
	case RA_UART_DATA_BITS_8:
		return ra_true;
		break;

	default:
		return ra_false;
		break;
	}
}

  static
ra_bool uart_parity_is_valid(ra_uart_parity_t parity)
{
	switch (parity)
	{
	case RA_UART_PARITY_ODD:
	case RA_UART_PARITY_EVEN:
	case RA_UART_PARITY_NONE:
		return ra_true;
		break;

	default:
		return ra_false;
		break;
	}
}

  static
ra_bool uart_stop_bits_is_valid(ra_uart_stop_bits_t stop_bits)
{
	switch (stop_bits)
	{
	case RA_UART_STOP_BITS_1:
	case RA_UART_STOP_BITS_15:
	case RA_UART_STOP_BITS_2:
		return ra_true;
		break;

	default:
		return ra_false;
		break;
	}
}

  static
void  uart_send_data_format(ra_uart_id_t uart_id, char *str, size_t len)
{
	if (uart_id == RA_UART0)
	{
		uart0_tx_buffer(str, (uint16)len);

		os_timer_disarm(&g_uart0_timer);
		os_timer_setfn(&g_uart0_timer, (os_timer_func_t *)uart0_timing_send_cb, NULL);
		os_timer_arm(&g_uart0_timer, uart0_send_interval, 0);
	}
	else
	{
		uart1_tx_buffer(str, (uint16)len);

		os_timer_disarm(&g_uart1_timer);
		os_timer_setfn(&g_uart1_timer, (os_timer_func_t *)uart1_timing_send_cb, NULL);
		os_timer_arm(&g_uart1_timer, uart1_send_interval, 0);
	}
	//debug_print("uart send uart[%d], heap = %d\n", uart_id, system_get_free_heap_size());
}

/*
send one data packaget
*/
  static
void execute_uart_send_1(ra_uart_id_t uart_id, GList **p_data_list)
{
	if unlikely(*p_data_list == NULL)
	{
		goto none;
	}
	GList * current = *p_data_list;
	dynamic_param_t *data = (dynamic_param_t *)(*p_data_list)->data;
	uart_send_data_format(uart_id, (char *)data->u.value_raw.src, data->u.value_raw.len);
	dynamic_param_destory(data);
	*p_data_list = g_list_delete_link(*p_data_list, current);
	goto done;
done:
	return;
none:
	if (uart_id == RA_UART0)
	{
		device_state_store(STATE_UART0_SEND_QUEUE_FINISH, 1);
	}
	else
	{
		device_state_store(STATE_UART1_SEND_QUEUE_FINISH, 1);
	}
	return;
}

  static
void uart0_timing_send_cb(void *arg)
{
	os_timer_disarm(&g_uart0_timer);

	device_state_store(STATE_UART0_SEND_QUEUE_FINISH, 0);
	execute_uart_send_1(RA_UART0, &uart0_send_data_list);

}

  static
void uart1_timing_send_cb(void *arg)
{
	os_timer_disarm(&g_uart1_timer);

	device_state_store(STATE_UART1_SEND_QUEUE_FINISH, 0);
	execute_uart_send_1(RA_UART1, &uart1_send_data_list);

}

  inline
static void uart_list_data_clear_handler(gpointer data)
{
	free(data);
}

  inline
static void uart_send_data_queue_clear(GList *list)
{
	g_list_free_full(list, &uart_list_data_clear_handler);
}

  static
int uart0_send_data(char *buf, ra_size_t len)
{
	dynamic_param_t *data = NULL;
	if likely(device_state_load(STATE_UART0_SEND_QUEUE_FINISH) == 1)
	{
		device_state_store(STATE_UART0_SEND_QUEUE_FINISH, 0);
		uart_send_data_format(RA_UART0, buf, len);
	}
	else
	{
		/*add send queue*/
		ASSERT(system_get_free_heap_size() < SYSTEM_RESERVE_HEAP_SIZE);

		data = dynamic_param_new(DYNAMIC_PARAM_TYPE_RAW, buf, len);
		ASSERT(data == NULL);
		
		uart0_send_data_list = g_list_append(uart0_send_data_list, (gpointer)data);
		ASSERT(uart0_send_data_list == NULL);

	}
	goto done;
done:
	return RA_SUCCESS;
fail:
	if (uart0_send_data_list != NULL)
	{
		uart_send_data_queue_clear(uart0_send_data_list);
	}
	uart0_send_data_list = NULL;
	dynamic_param_destory(data);
	return RA_ERROR_INTERNAL;
}

  static
int uart1_send_data(char *buf, ra_size_t len)
{
	dynamic_param_t *data = NULL;

	if likely(device_state_load(STATE_UART1_SEND_QUEUE_FINISH) == 1)
	{
		device_state_store(STATE_UART1_SEND_QUEUE_FINISH, 0);
		uart_send_data_format(RA_UART1, buf, len);
	}
	else
	{
		/*add send queue*/
		ASSERT(system_get_free_heap_size() < SYSTEM_RESERVE_HEAP_SIZE);
		data = dynamic_param_new(DYNAMIC_PARAM_TYPE_RAW, buf, len);
		ASSERT(data == NULL);

		uart1_send_data_list = g_list_append(uart1_send_data_list, (gpointer)data);
		ASSERT(uart1_send_data_list == NULL);

	}
	goto done;
done:
	return RA_SUCCESS;
fail:
	if (uart1_send_data_list != NULL)
	{
		uart_send_data_queue_clear(uart1_send_data_list);
	}
	uart1_send_data_list = NULL;
	dynamic_param_destory(data);
	return RA_ERROR_INTERNAL;
}

 
int ra_uart_recv_enable(ra_uart_id_t uart_id)
{
	if (uart_id != RA_UART0)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	uart0_set_enable();
	return RA_SUCCESS;
}

 
int ra_uart_recv_disable(ra_uart_id_t uart_id)
{
	if (uart_id != RA_UART0)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	uart0_set_disable();
	return RA_SUCCESS;
}

 
int ra_uart_send_data(ra_uart_id_t uart_id, char *buf, ra_size_t len)
{
	if (uart_id_is_valid(uart_id) == ra_false)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	if (uart_id == RA_UART0)
	{
		return uart0_send_data(buf, len);
	}
	else
	{
		return uart1_send_data(buf, len);
	}
}

 
int ra_uart_set_sending_queue_interval(ra_uart_id_t uart_id, ra_uint32_t interval)
{
	if ((uart_id_is_valid(uart_id) == ra_false) || (interval < 10))
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	(uart_id == RA_UART0) ? (uart0_send_interval = interval) : (uart1_send_interval = interval);	

	return RA_SUCCESS;
}

 
int ra_uart_set_rate(ra_uart_id_t uart_id, ra_uart_rate_t rate)
{
	if ((uart_id_is_valid(uart_id) == ra_false) || (uart_baudrate_is_valid(rate) == ra_false))
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	uart_set_baudrate(uart_id, rate);

	return RA_SUCCESS;
}

 
int ra_uart_set_data_bits(ra_uart_id_t uart_id, ra_uart_data_bits_t data_bits)
{
	if ((uart_id_is_valid(uart_id) == ra_false) 
		|| (uart_data_bits_is_valid(data_bits) == ra_false))
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	uart_set_data_bits(uart_id, data_bits);

	return RA_SUCCESS;
}

 
int ra_uart_set_stop_bits(ra_uart_id_t uart_id, ra_uart_stop_bits_t stop_bits)
{
	if ((uart_id_is_valid(uart_id) == ra_false) || (uart_stop_bits_is_valid(stop_bits) == ra_false))
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	uart_set_stop_bits(uart_id, stop_bits);

	return RA_SUCCESS;
}

 
int ra_uart_set_parity(ra_uart_id_t uart_id, ra_uart_parity_t parity)
{
	if ((uart_id_is_valid(uart_id) == ra_false) || (uart_parity_is_valid(parity) == ra_false))
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}
	uart_set_parity(uart_id, parity);

	return RA_SUCCESS;
}

 
int ra_uart_set_recv_timeout(ra_uart_id_t uart_id, ra_uint32_t timeout)
{
	if (uart_id != RA_UART0 && timeout < 10)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	uart0_set_timeout(timeout);
	return RA_SUCCESS;
}

 
int ra_uart_set_recv_buffer_size_limit(ra_uart_id_t uart_id, ra_uint16_t len)
{
	if (uart_id != RA_UART0 && len > 256)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	uart0_set_recv_buffer_size_limit(len);
	return RA_SUCCESS;
}

 
int ra_uart_register_recv_callback(ra_uart_id_t uart_id, ra_recv_uart_data_callback_t callback)
{
	if (uart_id != RA_UART0)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	register_uart_recv_callback(callback);
	return RA_SUCCESS;
}

 
int ra_uart_set_terminal(ra_uart_id_t uart_id)
{
	if (uart_id_is_valid(uart_id) == ra_false)
	{
		return RA_ERROR_INVALID_ARGUMENTS;
	}

	if (uart_id == RA_UART0)
	{
		device_state_store(STATE_LOG_PRINT_ENABLE, 1);
		uart0_set_terminal();
	}
	else
	{
		device_state_store(STATE_LOG_PRINT_ENABLE, 1);
		uart1_set_terminal();
	}
	return RA_SUCCESS;
}