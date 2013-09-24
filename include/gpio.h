/* 
 * Copyright (c) 2013, Erik Svensson <erik.public@gmail.com>
 *
 * Handling of general purpose input / output pins using /sys/class/gpio
 */

#ifndef NRF24_GPIO_H
#define NRF24_GPIO_H

#include <stdint.h>

#define NRF24_GPIO_INPUT 0
#define NRF24_GPIO_OUTPUT 1

#define NRF24_GPIO_LOW 0
#define NRF24_GPIO_HIGH 1

/*
 * "Open" a GPIO port. Essentially exports the GPIO and sets the direction.
 * port, the port (pin) to open.
 * output, direction of the pin (0 == input, 1 == output)
 * returns NRF24_OK on success or NRF24_OPEN_FAILED if any file open failed.
 */
int32_t nrf24_gpio_open(const uint16_t port, const uint8_t output);

/*
 * "Close" a GPIO port. Essentially un-exports the port.
 * port, the port (pin) to close.
 * returns NRF24_OK on success or NRF24_OPEN_FAILED if any file open failed.
 */
int32_t nrf24_gpio_close(const uint16_t port);

/*
 * Read a GPIO port.
 * port, the port (pin) to read.
 * value, the value read.
 * returns the value on success or NRF24_OPEN_FAILED if any file open failed.
 */
int32_t nrf24_gpio_read(const uint16_t port, uint8_t *value);

/*
 * Write to a GPIO port.
 * port, the port (pin) to write.
 * value, the value to write.
 * returns NRF24_OK on success or NRF24_OPEN_FAILED if any file open failed.
 */
int32_t nrf24_gpio_write(const uint16_t port, const uint8_t value);

#endif /* NRF24_GPIO_H */
