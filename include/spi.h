/* 
 * Copyright (c) 2013-2014, Erik Svensson <erik.public@gmail.com>
 *
 * Handling of SPI through linux spi devices (/dev/spidev)
 */

#ifndef NRF24_SPI_H
#define NRF24_SPI_H

#include <stdint.h>

/* Open SPI device.
 * controller is the SPI controller number i.e. x in /dev/spidev<x>.<y>.
 * device is the SPI device number i.e. y in /dev/spidev<x>.<y>.
 * speed is the frequency of the SPI bus.
 * bits is the number of bits per word transfered.
 * mode is the SPI mode, see linux kernel documentation .
 *
 * returns a nrf24_spi handle which is >= 0 or a negative error code.
 */
int32_t nrf24_spi_open(const uint32_t controller, const uint32_t device, const uint32_t speed, const uint8_t bits
	, const uint8_t mode);

/* Close a handle opened with nrf24_spi_open.
 * handle is the handle returned by nrf24_spi_open.
 *
 * returns NRF24_OK on success or NRF24_INVALID_HANDLE if the handle is invalid.
 */
int32_t nrf24_spi_close(const int32_t handle);

/* Perform a SPI transfer
 * handle isthe handle returned by nrf24_spi_open.
 * tx is the transmitter buffer, must be of length len.
 * rx is the receiver buffer, must be of length len.
 * len is the transmitter and receiver buffer length.
 *
 * returns NRF24_OK on success, NRF24_INVALID_HANDLE if the handle is invalid, NRF24_INVALID_ARGUMENT if any other
 * argument is invalid or NRF24_TRANSFER_FAILED if the transfer failed.
 */
int32_t nrf24_spi_transfer(const int32_t handle, uint8_t *tx, uint8_t *rx, const uint16_t len);

#endif /* NRF24_SPI_H */
