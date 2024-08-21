#include <stdio.h>
#include "utils/timer/time_utils.h"
#include "utils/timer/timer.h"

void timer_init(struct timer *timer, void (*handler)(struct timer *))
{
	timer->next = NULL;
	timer->time_ms = 0;
	timer->handler = handler;
}

#define MAX_TIMER_NUM 4096
void timer_cancel(struct timer_head *head, struct timer *timer)
{
	struct timer **prev;
	struct timer *node;
	uint16_t timer_num = 0;
	unsigned long long last_time = 0;

	if (!timer_active(timer)) {
		return;
	}
	for (prev = &head->first; (node = *prev) != NULL; prev = &node->next) {
		/*
		 * When the maximum number of timer is exceeded,
		 * do the time check and repair timer head list
		 */
		if (timer_num < MAX_TIMER_NUM) {
			timer_num++;
		} else {
			if (last_time > node->time_ms) {
				*prev = NULL;
				break;
			}
			last_time = node->time_ms;
		}
		if (node == timer) {
			*prev = node->next;
			node->time_ms = 0;
			break;
		}
	}
}

void timer_set(struct timer_head *head, struct timer *timer, uint64_t ms)
{
	struct timer **prev;
	struct timer *node;
	unsigned long long time;

	time = time_mtime_ms() + ms;
	timer_cancel(head, timer);
	timer->time_ms = time;

	for (prev = &head->first; (node = *prev) != NULL; prev = &node->next) {
		if (time < node->time_ms) {
			break;
		}
	}
	*prev = timer;
	timer->next = node;
}

void timer_reset(struct timer_head *head, struct timer *timer,
	void (*handler)(struct timer *), uint64_t ms)
{
	timer_cancel(head, timer);
	timer_init(timer, handler);
	timer_set(head, timer, ms);
}


uint64_t timer_delay_get_ms(struct timer *timer)
{
	if (!timer->time_ms) {
		return 0;
	}
	return timer->time_ms - time_mtime_ms();
}

int64_t timer_advance(struct timer_head *head)
{
	struct timer *node;
	uint64_t mtime = 0;

	while ((node = head->first) != NULL) {
		/*
		 * Return if the next timeout is in the future, but re-check
		 * the current time in case the last handler took a while.
		 */
		if (node->time_ms > mtime) {
			mtime = time_mtime_ms();
			if (node->time_ms > mtime) {
				return node->time_ms - mtime;
			}
		}
		head->first = node->next;
		node->time_ms = 0;
		node->next = NULL;
		node->handler(node);
	}
	return -1;
}
