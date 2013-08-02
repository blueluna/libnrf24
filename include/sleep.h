/* 
 * Copyright (c) 2013, Erik Svensson <erik.public@gmail.com>
 */

#ifndef NRF24_SLEEP_H
#define NRF24_SLEEP_H

#include <stdint.h>

void nrf24_msleep(const uint32_t milliseconds);
void nrf24_usleep(const uint32_t microseconds);

#endif /* NRF24_SLEEP_H */
