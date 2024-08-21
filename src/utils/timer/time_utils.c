#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include "utils/timer/time_utils.h"

/*
 * Get time since boot in milliseconds.  Returns a 64-bit value, eliminating
 * rollover concerns.  This is assumed low-overhead (no system calls)
 * using shared kernel page.
 */
uint64_t time_mtime_ms(void)
{
	struct timespec now;

	if (clock_gettime(CLOCK_MONOTONIC, &now)) {
		assert(false);
	}
	return ((uint64_t)now.tv_sec) * 1000 + (uint64_t)(now.tv_nsec / 1000000);
}
