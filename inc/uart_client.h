#ifndef __UART_CLIENT_H__
#define __UART_CLIENT_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "uart_msg_def.h"

#ifdef __cplusplus
extern "C" {
#endif /** __cplusplus*/

typedef struct uart_client uart_client_t;

/**
 * Initialize uart client.
 *
 * \param c: the pointer of uart_client.
 * \return: zero for success.
 */
int uart_client_init(uart_client_t *c);

/**
 * Finalize uart client.
 *
 * \param c: the pointer of uart_client.
 */
void uart_client_final(uart_client_t *c);

/**
 * Start uart client.
 *
 * \param c: the pointer of uart_client.
 * \param int_thd: if enable the internal thread or not.
 * \return: zero for success.
 */
int uart_client_start(uart_client_t *c, bool int_thd);

/**
 * Polling uart data, if the int_thd filed is true, that means user do not need to call this function.
 *
 * \param c: the pointer of uart_client.
 */
void uart_client_poll(uart_client_t *c, uint64_t timeout_ms);

/**
 * Stop uart client.
 * 
 * \param c: the pointer of uart_client.
 */
void uart_client_stop(uart_client_t *c);

/**
 * Register an uart msg process unit.
 * 
 * \param c: the pointer of uart_client.
 * \param msg_type: the type of the process unit. example: UART_MSG_TYPE_WHEEL<->WCU(wheel control unit)
 * \param header: byte indicating the beginning of the uart message.
 * \param end: byte indicating the end of a uart message.
 * \param cb: the callback will be called when data is received.
 * \param ctx: user data.
 */
int uart_client_unit_register(uart_client_t *c, uart_msg_type_e type, uint8_t h, uint8_t e, uart_msg_cb_t cb, const void *ctx);

/**
 * Deregister an uart msg process unit.
 */
void uart_client_unit_deregister(uart_client_t *c, uint8_t h);

/**
 * Send message via uart.
 *
 * \param c: the pointer of uart client.
 * \param payload: the pointer of the message.
 * \param len: the length of the payload.
 * \return : zero for success.
 */
int uart_client_send(uart_client_t *c, const uint8_t *payload, int len);
#ifdef __cplusplus
}
#endif /** __cplusplus*/
#endif /** __UART_CLIENT_H__*/
