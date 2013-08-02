/* 
 * Copyright (c) 2013, Erik Svensson <erik.public@gmail.com>
 */

#ifndef NRF24_GPIO_H
#define NRF24_GPIO_H

#include <stdint.h>

#define NRF24_GPIO_INPUT 0
#define NRF24_GPIO_OUTPUT 1

#define NRF24_GPIO_LOW 0
#define NRF24_GPIO_HIGH 1

int32_t nrf24_gpio_open(const uint16_t port, const uint8_t output);
int32_t nrf24_gpio_close(const uint16_t port);

int32_t nrf24_gpio_read(const uint16_t port, uint8_t *value);
int32_t nrf24_gpio_write(const uint16_t port, const uint8_t value);

#endif /* NRF24_GPIO_H */
