#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "utils/logger/log.h"
#include "utils/timer/timer.h"
#include "uart_client.h"
#include "wcu.h"

volatile static bool b_exit = false;
static struct timer_head g_timer_head;
extern uart_client_t g_uart_client;
static void signal_handler(int sig)
{
	log_warn("received signal %d.\n", sig);
	b_exit = true;
}

//reporting to user
static void wcu_rpm_reporter_cb_impl(wcu_t *wcu, int sum)
{
	log_debug("report sum of RPM to user [%d].\n", sum);
}

//getting rpm from wheel.
static void wcu_rpm_get_cb_impl(wcu_t *wcu, int16_t *rpm_w1, int16_t *rpm_w2, int16_t *rpm_w3, int16_t *rpm_w4)
{
	*rpm_w1 = 1;
	*rpm_w2 = 1;
	*rpm_w3 = 1;
	*rpm_w4 = 1;
}

static void uart_msg_cb_impl(uart_msg_type_e e, const void *ctx, uint8_t *payload, uint32_t len)
{
	assert(ctx);
	assert(payload);

	wcu_recv_proc((wcu_t *)ctx, payload, len);
}

int main()
{
	int32_t ret = 0;

	if (0 != log_init()) {
		printf("log_init failed.\n");
		return -1;
	}

	log_set_thread_name("WCU");
	log_set_level("DEBUG");
	log_set_stdout(true);

	log_info("main entry.\n");

	signal(SIGINT, signal_handler);

	memset(&g_timer_head, 0, sizeof(struct timer_head));
	if (uart_client_init(&g_uart_client)) {
		log_error("uart_client_init failed.\n");
		return -1;
	}

	if (uart_client_start(&g_uart_client, false)) {
		log_error("uart_client_start failed.\n");
		return -1;
	}

	wcu_t *wcu = wcu_create(&g_uart_client, WCU_MODE_HOST, &g_timer_head, wcu_rpm_reporter_cb_impl, wcu_rpm_get_cb_impl, 1000, 100);
	assert(wcu);

	if (0 != uart_client_unit_register(&g_uart_client, UART_MSG_TYPE_WHEEL, WCU_HEADER, WCU_END, uart_msg_cb_impl, wcu)) {
		log_error("uart_client_unit_register failed.\n");
		return -1;
	}


	while (!b_exit) {

		timer_advance(&g_timer_head);
		
		uart_client_poll(&g_uart_client, 0);

		usleep(20 * 1000);
	}
	
	if (wcu)
		wcu_destroy(wcu);

	uart_client_stop(&g_uart_client);
	uart_client_final(&g_uart_client);	

	log_info("main exit.\n");

	log_term();
	return ret;
}
