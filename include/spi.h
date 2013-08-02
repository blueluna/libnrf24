/* 
 * Copyright (c) 2013, Erik Svensson <erik.public@gmail.com>
 */

#ifndef NRF24_SPI_H
#define NRF24_SPI_H

#include <stdint.h>

int32_t nrf24_spi_open(const uint32_t controller, const uint32_t device, const uint32_t speed, const uint8_t bits, const uint8_t mode);
int32_t nrf24_spi_close(const int32_t handle);

int32_t nrf24_spi_transfer_byte(const int32_t handle, const uint8_t tx, uint8_t *rx);
int32_t nrf24_spi_transfer(const int32_t handle, uint8_t *tx, uint8_t *rx, const uint16_t len);

#endif /* NRF24_SPI_H */
