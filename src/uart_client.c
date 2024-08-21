#include "uart_client.h"
#include "utils/logger/log.h"
#include "utils/intaccess.h"
#include "wcu.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define UART_CLIENT_DEFAULT_TIMEOUT_MS 10
#define UART_CLIENT_BUFFER_SIZE 1024

/**
 * The class for conmunication unit via uart.
 */
typedef struct unit_ctx {
	uint8_t h;
	uint8_t e;
	uart_msg_type_e type;
	uart_msg_cb_t cb;
	const void *ctx;
} unit_ctx_t;

typedef enum uart_state {
	UART_CLIENT_STATE_CANCEL = 0,
	UART_CLIENT_STATE_READY,
	UART_CLIENT_STATE_RUNNING,
	UART_CLIENT_STATE_UNKOWN
} uart_state_e;

typedef struct uart_client {
	bool enable_internal_thread;
	uart_state_e state;
	pthread_t pid;

	unit_ctx_t unit_ctx[0xff];
} uart_client_t;

uart_client_t g_uart_client;

#if 0 //internal polling thread.
static void uart_client_thread(void *arg)
{
	assert(arg);

	uart_client_t *c = (uart_client_t *)arg;

	if (!c->enable_internal_thread) {
		log_error("uart_client disable internal thread.\n");
		return ;
	}

	while (c->state == UART_CLIENT_STATE_RUNNING) {
		uart_client_poll(c, 0);
		usleep(UART_CLIENT_DEFAULT_TIMEOUT_MS * 1000);
	}

	log_info("uart_internal thread create success.\n");
}
#endif

int uart_client_init(uart_client_t *c)
{
	assert(c);

	memset(c, 0, sizeof(*c));
	c->state = UART_CLIENT_STATE_READY;

#if 0
	uart_open();
#endif

	log_info("%s .\n", __func__);
	return 0;
}

void uart_client_final(uart_client_t *c)
{
	assert(c);

	memset(c, 0, sizeof(*c));
	c->state = UART_CLIENT_STATE_CANCEL;

#if 0
	uart_close();
#endif

	log_info("%s .\n", __func__);
}

int uart_client_start(uart_client_t *c, bool int_thd)
{
	assert(c);
	assert(c->state == UART_CLIENT_STATE_READY);

	if (c->state != UART_CLIENT_STATE_READY) {
		log_error("%s, the state of uart client is wrong, state[%d].\n", __func__, c->state);
		return -1;
	}

	log_info("%s int_thd[%d]\n", __func__, int_thd);

	c->enable_internal_thread = int_thd;
	if (int_thd) {
		/** if int_thd is enable, create a internal thread here.*/
		/**
		 * pthread_create
		 */
	}

	c->state = UART_CLIENT_STATE_RUNNING;
	return 0;
}

void uart_client_poll(uart_client_t *c, uint64_t timeout_ms)
{
	assert(c);

	if (c->state != UART_CLIENT_STATE_RUNNING)
		return ;

#if 0
	uart_read();
#else
	struct wcu_rsp {
		uint8_t h;
		int16_t w1;
		int16_t w2;
		int16_t w3;
		int16_t w4;
		uint8_t e;
	} __attribute__((packed));
	
	struct wcu_rsp rsp = {
		.h = WCU_HEADER,
		.w1 = -1,
		.w2 = 1,
		.w3 = 1,
		.w4 = 1,
		.e = WCU_END
	};
	//recv simu data from device.
		
	if (c->unit_ctx[WCU_HEADER].cb) {
		c->unit_ctx[WCU_HEADER].cb(UART_MSG_TYPE_WHEEL, c->unit_ctx[0x55].ctx, (uint8_t *)&rsp, sizeof(struct wcu_rsp));
	}
#endif
}


void uart_client_stop(uart_client_t *c)
{
	assert(c);
	assert(c->state == UART_CLIENT_STATE_RUNNING);

	if (c->state != UART_CLIENT_STATE_RUNNING) {
		log_error("the state of uart client is wrong, state[%d].\n", c->state);
		return ;
	}

	/** if it is a internal thread in uart_client, destroy the thread here.*/
	/**
	 * pthread_destroy
	 */

	c->state = UART_CLIENT_STATE_READY;
	c->enable_internal_thread = false;
	log_info("%s .\n", __func__);
}

int uart_client_unit_register(uart_client_t *c, uart_msg_type_e type, uint8_t h, uint8_t e, uart_msg_cb_t cb, const void *ctx)
{
	assert(c);

	if (c->state == UART_CLIENT_STATE_CANCEL) {
		log_error("uart client state is wrong.\n");
		return -1;
	}

	log_info("uart_msg_type[%d] header[%u] e[%u].\n", type, h, e);

	c->unit_ctx[h].type = type;
	c->unit_ctx[h].h = h;
	c->unit_ctx[h].e = e;
	c->unit_ctx[h].cb = cb;
	c->unit_ctx[h].ctx = ctx;
	return 0;
}

void uart_client_unit_deregister(uart_client_t *c, uint8_t h)
{
	assert(c);

	memset(&c->unit_ctx[h], 0, sizeof(unit_ctx_t));
}

int uart_client_send(uart_client_t *c, const uint8_t *payload, int len)
{
	assert(c);
	assert(payload);
#if 0	
	if (0 >= uart_send(payload, len)) {
		return -1;
	}
#endif
	return 0;
}
