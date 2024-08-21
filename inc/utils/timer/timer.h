#ifndef __UTILS_TIMER_H__
#define __UTILS_TIMER_H__
#include <stdint.h>

struct timer {
	struct		timer *next;
	uint64_t	time_ms;
	void (*handler)(struct timer *);
};

struct timer_head {
	struct timer *first;
};

static inline int timer_active(const struct timer *timer)
{
	return timer->time_ms != 0;
}

void timer_init(struct timer *, void (*handler)(struct timer *));
void timer_cancel(struct timer_head *, struct timer *);
void timer_set(struct timer_head *, struct timer *, uint64_t delay_ms);
void timer_reset(struct timer_head *, struct timer *,
	void (*handler)(struct timer *), uint64_t delay_ms);
uint64_t timer_delay_get_ms(struct timer *);

/*
 * Handle timers and return delay until next timer fires.
 * Return -1 if no timers scheduled.
 */
int64_t timer_advance(struct timer_head *);

#endif /* __UTILS_TIMER_H__ */
