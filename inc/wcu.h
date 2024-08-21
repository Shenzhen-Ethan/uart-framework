#ifndef __WCU_H__
#define __WCU_H__

#include <stdint.h>
#include "utils/timer/timer.h"
#include "uart_client.h"

#ifdef __cplusplus
extern "C" {
#endif /** __cplusplus*/

#define WCU_HEADER	(uint8_t)(0X55)
#define WCU_END		(uint8_t)(0XAA)

typedef enum wcu_mode {
	WCU_MODE_UNKOWN = 0,
	WCU_MODE_HOST,
	WCU_MODE_DEVICE
} wcu_mode_e;

/**
 * Wheel control/conmunication unit.(WCU)
 */
typedef struct wcu wcu_t;

/**
 * The callback function for WCU reports the sum of RPM to user.
 * Only for host mode.
 */
typedef void (*wcu_rpm_reporter_cb_t)(wcu_t *wcu, int sum);

/**
 * The callback function for WCU to get the RPM of each wheel.
 * Only for device mode.
 */
typedef void (*wcu_rpm_get_cb_t)(wcu_t *wcu, int16_t *rpm_w1, int16_t *rpm_w2, int16_t *rpm_w3, int16_t *rpm_w4);

/**
 * Create a wcu.
 *
 * \param mode: HOST OR DEVICE.
 * \param rcb: should not be null if the mode is HOST.
 * \param gcb: should not be null if the mode is DEVICE.
 * \param r_freq_ms: the frequency of reporting the sum of RPM.
 * \parm g_freq_ms: the frequency of getting the RPM of THE WHEEL.
 * \return : the pointer of wcu.
 */
wcu_t *wcu_create(uart_client_t *c, wcu_mode_e mode, struct timer_head *timer, wcu_rpm_reporter_cb_t rcb,
	wcu_rpm_get_cb_t gcb, uint64_t r_freq_ms, uint64_t g_freq_ms);

/**
 * Destroy a wcu object.
 *
 * \param wcu: the pointer of wcu object.
 */
void wcu_destroy(wcu_t *wcu);

/**
 * Handle the received data.
 *
 * \param wcu: the pointer of wcu object.
 * \
 */
void wcu_recv_proc(wcu_t *wcu, uint8_t *payload, uint32_t len);

/**
 * HOST controling message.
 *
 * \param wcu: the pointer of wcu object.
 * \param linear:
 * \param angular:
 */
int wcu_host_send_proc(wcu_t *wcu, int16_t linear, int16_t angular);

/**
 * Device responsed message.
 *
 * \param wcu: the pointer of wcu object.
 * \param linear:
 * \param angular:
 */
int wcu_dev_send_proc(wcu_t *wcu, int16_t rpm_w1, int16_t rpm_w2, int16_t rpm_w3, int16_t rpm_w4);

#ifdef __cplusplus
}
#endif /** __cplusplus*/
#endif /** __WCU_H__*/
