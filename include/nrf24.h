/* 
 * Copyright (c) 2013, Erik Svensson <erik.public@gmail.com>
 *
 * Handling communication with nRF24L01 chips using user space SPI and GPIO
 */

#ifndef NRF24_H
#define NRF24_H

#include <stdint.h>
#include "nrf24_constants.h"

void nrf24_version(uint16_t *major, uint16_t *minor, uint16_t *fix, char *commit, const int32_t commit_len);

typedef struct nrf24_ctx_s {
	int32_t spi_handle; /* A handle to the SPI interface */
	uint16_t ce_pin; /* The GPIO index for the CE signal */
	uint8_t *rx_buf; /* A 33 byte receiver buffer */
	uint8_t *tx_buf; /* A 33 byte transmitter buffer */
} nrf24_ctx_t;

typedef nrf24_ctx_t* nrf24_handle;

/* Open an nrf24 context
 * spi_handle is a SPI handle created with nrf24_spi_open
 * ce_pin is the pin number for the pin connected to the nrf24xxx CE signal
 *
 * returns a nrf24_handle for use with subsequent operations with the nrf24xxx module
 */
nrf24_handle nrf24_open(const int32_t spi_handle, const uint16_t ce_pin);

/* A note on the handle
 * Following functions takes the handle created with the above nrf24_open. If
 * the handle is invalid (null) the function shall return NRF24_INVALID_HANDLE.
 */

/* Close an nrf24 context
 * handle is a handle created with nrf24_open that shall be closed.
 *
 * returns NRF24_OK on success.
 */
int32_t nrf24_close(nrf24_handle handle);

/* Read a nrf24 register, see NRF24_REG_ in nrf24_constants.h and the nrf24 documentation for available registers.
 * address is the address of the register that will be read.
 * value is the value of the requested register.
 *
 * returns NRF24_OK on success, NRF24_INVALID_ARGUMENT if any argument is invalid or NRF24_TRANSFER_FAILED if the SPI
 * transfer failed.
 */
int32_t nrf24_get_register(nrf24_handle handle, const uint8_t address, uint8_t *value);

/* Write a nrf24 register, see NRF24_REG_ in nrf24_constants.h and the nrf24 documentation for available registers.
 * address is the address of the register that will be read.
 * value is the value to write to the register.
 *
 * returns NRF24_OK on success, NRF24_INVALID_ARGUMENT if any argument is invalid or NRF24_TRANSFER_FAILED if the SPI
 * transfer failed.
 */
int32_t nrf24_set_register(nrf24_handle handle, const uint8_t address, const uint8_t value);

/* Following functions read and write registers, thus have the same return values as the above nrf24_get_register and
 * nrf24_set_register.
 */

/* Power up the module */
int32_t nrf24_powerup(nrf24_handle handle);

/* Power down the module */
int32_t nrf24_powerdown(nrf24_handle handle);

/* Get the current CRC configuration
 * bytes is the number of CRC bytes.
 */
int32_t nrf24_get_crc(nrf24_handle handle, uint8_t *bytes);

/* Configure CRC, CRC is either, disabled (0), 1 byte (1) or 2 bytes (2)
 * bytes is the number of CRC bytes.
 */
int32_t nrf24_set_crc(nrf24_handle handle, const uint8_t bytes);

/* Get the current radio channel
 * channel is the current radio channel.
 */
int32_t nrf24_get_channel(nrf24_handle handle, uint8_t *channel);

/* Set the radio channel, there are 128 channels (0-127).
 * channel is the radio channel to be set.
 */
int32_t nrf24_set_channel(nrf24_handle handle, const uint8_t channel);

/* Get the current data rate in kbps (Kilo bits per second).
 * data_rate is the current channel speed in kbps.
 */
int32_t nrf24_get_data_rate(nrf24_handle handle, uint16_t *data_rate);

/* Set the data rate, the data rate is described i kbps (Kilo bits per second).
 * data_rate is the channel speed to set. Available values are 250, 1000 and 2000.
 */
int32_t nrf24_set_data_rate(nrf24_handle handle, const uint16_t data_rate);

/* Get the transmission power in dBm.
 * power is the current transmission power in dBm.
 */
int32_t nrf24_get_power(nrf24_handle handle, int8_t *power);

/* Set the transmission power in dBm.
 * power is the current transmission power in dBm. Available values are -18, -12, -6 and 0dBm.
 */
int32_t nrf24_set_power(nrf24_handle handle, const int8_t power);

/* Get the payload length for a receiver data pipe.
 * pipe is the data pipe to get the payload length for
 * length is the current payload length for the data pipe
 */
int32_t nrf24_get_rx_payload_length(nrf24_handle handle, const uint8_t pipe, uint8_t *length);

/* Get the payload length for a receiver data pipe.
 * pipe is the data pipe to set the payload length for
 * length is the payload length for the data pipe. 0-32 bytes.
 */
int32_t nrf24_set_rx_payload_length(nrf24_handle handle, const uint8_t pipe, const uint8_t length);

/* Get the auto retransmit retry count and retry delay.
 * retries is the current number of retransmit retries.
 * delay is the current retransmit retry delay.
 */
int32_t nrf24_get_auto_retransmit(nrf24_handle handle, uint8_t *retries, uint16_t *delay);

/* Set the auto retransmit retry count and retry delay.
 * retries is the number of retransmit retries. 0-15, where 0 disables auto retransmit.
 * delay is the retransmit retry delay. delay in 250us steps from 250us to 4000us.
 */
int32_t nrf24_set_auto_retransmit(nrf24_handle handle, const uint8_t retries, const uint16_t delay);

/* Clear status. Clears data ready, data sent and max retry flags. */
int32_t nrf24_clear_status(nrf24_handle handle);

/* A note about receiver addresses
 * The receiver have 6 data pipes each with an address. An address is up to 5 bytes long. The first two pipes (0 and 1)
 * have full length addresses while the latter 4 share the upper 4 bytes with pipe 1. The address length is acctually
 * configurable but thats not supported in libnrf24.
 * If autmatic acknowledge is used the data pipe 0 address should be the same as the transmitter address.
 */
/* Get the receiver data pipe address. 
 * pipe is the data pipe to get the address for
 * address is the buffer that receives the address, must be at least 5 bytes long.
 * address_len is the length of the address buffer, must be at least 5.
 */
int32_t nrf24_get_rx_address(nrf24_handle handle, const uint8_t pipe, uint8_t *address, const uint32_t address_len);

/* Set the receiver data pipe address.
 * pipe is the data pipe to get the address for. Only data pipe 0 and 1 supported.
 * address is the buffer with the address to write, must be at least 5 bytes long.
 * address_len is the length of the address buffer, must be at least 5.
 */
int32_t nrf24_set_rx_address(nrf24_handle handle, const uint8_t pipe, const uint8_t *address, const uint32_t address_len);

/* Set the receiver data pipe address.
 * pipe is the data pipe to get the address for. Only data pipe 2 to 5 supported.
 * address is the least significant byte in the address for the selected data pipe. The most significant bytes are
 * shared with data pipe 1.
 */
 int32_t nrf24_set_rx_address_byte(nrf24_handle handle, const uint8_t pipe, const uint8_t address);

/* Get the transmitter address. 
 * address is the buffer that receives the address, must be at least 5 bytes long.
 * address_len is the length of the address buffer, must be at least 5.
 */
int32_t nrf24_get_tx_address(nrf24_handle handle, uint8_t *address, const uint32_t address_len);

/* Set the transmitter address.
 * address is the buffer with the address to write, must be at least 5 bytes long.
 * address_len is the length of the address buffer, must be at least 5.
 */
int32_t nrf24_set_tx_address(nrf24_handle handle, const uint8_t *address, const uint32_t address_len);

/* Write to the transmitter payload buffer.
 * data is the data buffer to write.
 * len is the number of bytes to write.
 *
 * Returns the number of bytes written on success.
 */
int32_t nrf24_write_tx_payload(nrf24_handle handle, const uint8_t *data, const uint8_t len);

/* Read from the receiver payload buffer.
 * data is the data buffer to receive the data.
 * len is the size of the data buffer.
 *
 * Returns the number of bytes read on success.
 */
int32_t nrf24_read_rx_payload(nrf24_handle handle, uint8_t *data, const uint8_t len);

/* Flush the receiver FIFO. */
int32_t nrf24_flush_rx(nrf24_handle handle);

/* Flush the transmitter FIFO. */
int32_t nrf24_flush_tx(nrf24_handle handle);

/* Enter receiver mode where the radio listens for new transfers. Disables the transmitter. */
int32_t nrf24_start_listen(nrf24_handle handle);

/* Exit receiver mode. Enables the transmitter. */
int32_t nrf24_stop_listen(nrf24_handle handle);

/* Get status flags.
 * data_ready is 1 when more data is available.
 * data_sent is 1 when data has been sent.
 * max_retry is 1 when a transmission has failed because of retry count.
 */
int32_t nrf24_get_status(nrf24_handle handle, uint8_t *data_ready, uint8_t *data_sent, uint8_t *max_retry);

/* Send data. Must be used when NOT listening.
 * data is the data buffer to send.
 * len is the number of bytes to send.
 *
 * Returns NRF24_OK on success, NRF24_MAX_RETRIES if the transmission failed because of retry count or other.
 */
int32_t nrf24_send(nrf24_handle handle, const uint8_t *data, const uint8_t len);

/* Receive data. Must be used when listening.
 * data is the data buffer to receive the data.
 * len is the size of the data buffer.
 *
 * Returns the number of bytes received on success, NRF24_NOT_LISTENING if the radio is in transmitter mode, 
 * NRF24_NO_DATA if no data is available or other.
 */
int32_t nrf24_receive(nrf24_handle handle, uint8_t *data, const uint8_t len);

#endif /* NRF24_H */
