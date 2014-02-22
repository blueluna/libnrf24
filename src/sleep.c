/* 
 * Copyright (c) 2013-2014, Erik Svensson <erik.public@gmail.com>
 *
 * Helper functions for sleeping
 */

#include "sleep.h"
#include <stddef.h>
#include <time.h>
#include <sys/time.h>

void nrf24_msleep(const uint32_t milliseconds)
{
	struct timespec req;
	uint32_t sec = milliseconds / 1000;
	uint32_t msec = milliseconds - (sec * 1000);
	req.tv_sec = sec;
	req.tv_nsec = msec * 1000000L; /* convert to nanoseconds */
	nanosleep(&req, (struct timespec *)NULL);
}

void nrf24_usleep(const uint32_t microseconds)
{
	struct timespec req;
	uint32_t sec = microseconds / 1000000;
	uint32_t usec = microseconds - (sec * 1000000);
	req.tv_sec = sec;
	req.tv_nsec = usec * 1000L; /* convert to nanoseconds */
	nanosleep(&req, (struct timespec *)NULL);
}
