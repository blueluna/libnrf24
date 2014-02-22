/* 
 * Copyright (c) 2013-2014, Erik Svensson <erik.public@gmail.com>
 *
 * Helper functions for sleeping
 */

#ifndef NRF24_SLEEP_H
#define NRF24_SLEEP_H

#include <stdint.h>

/* Sleep for milliseconds ... */
void nrf24_msleep(const uint32_t milliseconds);

/* Sleep for microseconds ... */
void nrf24_usleep(const uint32_t microseconds);

#endif /* NRF24_SLEEP_H */
