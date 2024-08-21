#include "wcu.h"
#include "utils/logger/log.h"
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
	(type *)( (char *)__mptr - offsetof(type,member) );})

//the cammand from host.
struct wcu_cmd {
	uint8_t h;
	int16_t linear;
	int16_t angular;
	uint8_t e;
} __attribute__((packed));
typedef struct wcu_cmd wcu_cmd_t;

//the reponse from device.
struct wcu_rsp {
	uint8_t h;
	int16_t w1;
	int16_t w2;
	int16_t w3;
	int16_t w4;
	uint8_t e;
} __attribute__((packed));
typedef struct wcu_rsp wcu_rsp_t;

typedef struct wcu {
	uart_client_t *c;
	struct timer_head *timer_head;		
	struct timer rtimer; //timer for reporting to user.
	struct timer gtimer; //timer for getting RPM from wheel.
	wcu_mode_e mode;
	wcu_rpm_reporter_cb_t rcb;
	wcu_rpm_get_cb_t gcb;
	uint64_t r_freq_ms; //reporting freqency.
	uint64_t g_freq_ms; //getting freqency.
	int16_t rpm_w1;
	int16_t rpm_w2;
	int16_t rpm_w3;
	int16_t rpm_w4;
	int16_t linear;
	int16_t angular;
} wcu_t;

static void wcu_reporter_cb(struct timer *timer)
{
	wcu_t *wcu = (wcu_t *)container_of(timer, struct wcu, rtimer);
	assert(wcu);

	if (!wcu->rcb) {
		log_warn("callback function is nullptr.\n");
		return ;
	}
	
	log_debug("rpm_w1[%d] rpm_w2[%d] rpm_w3[%d] rpm_w4[%d].\n", wcu->rpm_w1, wcu->rpm_w2, wcu->rpm_w3, wcu->rpm_w4);
	wcu->rcb(wcu, wcu->rpm_w1 + wcu->rpm_w2 + wcu->rpm_w3 + wcu->rpm_w4);
	wcu->rpm_w1 = 0;
	wcu->rpm_w2 = 0;
	wcu->rpm_w3 = 0;
	wcu->rpm_w4 = 0;

	timer_reset(wcu->timer_head, timer, wcu_reporter_cb, wcu->r_freq_ms);
}

static void wcu_get_cb(struct timer *timer)
{
	wcu_t *wcu = (wcu_t *)container_of(timer, wcu_t, gtimer);
	assert(wcu);

	if (!wcu->gcb) {
		log_warn("callback function is nullptr.\n");
		return ;
	}
	
	int16_t rpm_w1 = 0, rpm_w2 = 0, rpm_w3 = 0, rpm_w4 = 0;
	
	wcu->gcb(wcu, &rpm_w1, &rpm_w2, &rpm_w3, &rpm_w4);
	
	wcu->rpm_w1 += rpm_w1;
	wcu->rpm_w2 += rpm_w2;
	wcu->rpm_w3 += rpm_w3;
	wcu->rpm_w4 += rpm_w4;

	timer_reset(wcu->timer_head, timer, wcu_get_cb, wcu->g_freq_ms);
}

wcu_t *wcu_create(uart_client_t *c, wcu_mode_e mode, struct timer_head *timer, wcu_rpm_reporter_cb_t rcb,
	wcu_rpm_get_cb_t gcb, uint64_t r_freq_ms, uint64_t g_freq_ms)
{
	assert(timer);

	if ((mode == WCU_MODE_HOST && !rcb) || (mode == WCU_MODE_HOST && r_freq_ms == 0)) {
		log_error("args invalid.\n");
		return NULL;
	} else if ((mode == WCU_MODE_DEVICE && !gcb) || (mode == WCU_MODE_DEVICE && g_freq_ms == 0)) {
		log_error("args invalid.\n");
		return NULL;
	}

	wcu_t *wcu = (wcu_t *)malloc(sizeof(wcu_t));
	memset(wcu, 0, sizeof(wcu_t));

	wcu->c = c;
	wcu->timer_head = timer;
	wcu->mode = mode;
	wcu->rcb = rcb;
	wcu->gcb = gcb;
	wcu->r_freq_ms = r_freq_ms;
	wcu->g_freq_ms = g_freq_ms;

	if (mode == WCU_MODE_HOST) {
		timer_init(&wcu->rtimer, wcu_reporter_cb);
		timer_set(wcu->timer_head, &wcu->rtimer, r_freq_ms);
	} else {// WCU_MODE_DEVICE;
		timer_init(&wcu->gtimer, wcu_get_cb);
		timer_set(wcu->timer_head, &wcu->gtimer, g_freq_ms);
	}
	
	log_info("wcu create success.\n");
	return wcu;
}

/**
 * Destroy a wcu object.
 *
 * \param wcu: the pointer of wcu object.
 */
void wcu_destroy(wcu_t *wcu)
{
	assert(wcu);

	if (wcu->mode == WCU_MODE_HOST) {
		timer_cancel(wcu->timer_head, &wcu->rtimer);
	} else {
		timer_cancel(wcu->timer_head, &wcu->gtimer);
	}

	memset(wcu, 0, sizeof(wcu_t));
	free(wcu);

	log_info("wcu destroy success.\n");
}

/**
 * Handle the received data.
 *
 * \param wcu: the pointer of wcu object.
 * \
 */
void wcu_recv_proc(wcu_t *wcu, uint8_t *payload, uint32_t len)
{
	if (payload[0] != WCU_HEADER && payload[len - 1] != WCU_END) {
		log_error("the format of payload is wrong.\n");
		return ;
	}

	if (wcu->mode == WCU_MODE_DEVICE) {
		if (len != sizeof(wcu_cmd_t)) {
			log_error("the received msg from host is invalid.\n");
			return ;
		}

		wcu_cmd_t *cmd = (wcu_cmd_t *)payload;
		wcu->linear = cmd->linear;
		wcu->angular = cmd->angular; 
		return ;
	}

	//host
	if (len != sizeof(wcu_rsp_t)) {
		log_error("the received msg from device is invalid len[%d] rsp_len[%d].\n", len, sizeof(wcu_rsp_t));
		return ;
	}

	wcu_rsp_t *rsp = (wcu_rsp_t *)payload;
	wcu->rpm_w1 += rsp->w1;
	wcu->rpm_w2 += rsp->w2;
	wcu->rpm_w3 += rsp->w3;
	wcu->rpm_w4 += rsp->w4;
}

/**
 * HOST controling message.
 *
 * \param wcu: the pointer of wcu object.
 * \param linear:
 * \param angular:
 */
int wcu_host_send_proc(wcu_t *wcu, int16_t linear, int16_t angular)
{
	wcu_cmd_t cmd = {
		.h = WCU_HEADER,
		.linear = linear,
		.angular = angular,
		.e = WCU_END
	};

	return uart_client_send(wcu->c, (uint8_t *)&cmd, sizeof(cmd));
}

/**
 * Device responsed message.
 *
 * \param wcu: the pointer of wcu object.
 * \param linear:
 * \param angular:
 */
int wcu_dev_send_proc(wcu_t *wcu, int16_t rpm_w1, int16_t rpm_w2, int16_t rpm_w3, int16_t rpm_w4)
{
	wcu_rsp_t rsp = {
		.h = WCU_HEADER,
		.w1 = rpm_w1,
		.w2 = rpm_w2,
		.w3 = rpm_w3,
		.w4 = rpm_w4,
		.e = WCU_END
	};

	return uart_client_send(wcu->c, (uint8_t *)&rsp, sizeof(rsp));
}
