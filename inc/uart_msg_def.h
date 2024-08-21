#ifndef __UART_MSG_DEF_H__
#define __UART_MSG_DEF_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /** __cplusplus*/

typedef enum uart_msg_type {
	UART_MSG_TYPE_UNKOWN = 0,
	UART_MSG_TYPE_WHEEL = 1,
	/**
	 *
	 */
} uart_msg_type_e;

/**
 * The prototype of uart message callback function, the callback will be called when data is received.
 *
 * \param e: the type of the message.
 * \param ctx: user data.
 * \param payload: the valid data which received from the other side.
 * \param len: the length of the payload.
 * \return: non.
 */
typedef void (*uart_msg_cb_t)(uart_msg_type_e e, const void *ctx, uint8_t *payload, uint32_t len);

#ifdef __cplusplus
}
#endif /** __cplusplus*/
#endif /** __UART_MSG_DEF_H__*/
