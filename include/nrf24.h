#ifndef NRF24_H
#define NRF24_H

#include <stdint.h>
#include "nrf24_constants.h"

typedef struct nrf24_ctx_s {
  int32_t spi_handle;
  uint16_t ce_pin;
  uint8_t *rx_buf;
  uint8_t *tx_buf;
} nrf24_ctx_t;

typedef nrf24_ctx_t* nrf24_handle;

/* Open an nrf24 context
 */
nrf24_handle nrf24_open(const int32_t spi_handle, const uint16_t ce_pin);
int32_t nrf24_close(nrf24_handle handle);

int32_t nrf24_get_register(nrf24_handle handle, const uint8_t address, uint8_t *value);
int32_t nrf24_set_register(nrf24_handle handle, const uint8_t address, const uint8_t value);

int32_t nrf24_powerup(nrf24_handle handle);
int32_t nrf24_powerdown(nrf24_handle handle);

int32_t nrf24_get_crc(nrf24_handle handle, uint8_t *bytes);
int32_t nrf24_set_crc(nrf24_handle handle, const uint8_t bytes);

int32_t nrf24_get_channel(nrf24_handle handle, uint8_t *channel);
int32_t nrf24_set_channel(nrf24_handle handle, const uint8_t channel);

int32_t nrf24_get_data_rate(nrf24_handle handle, uint8_t *data_rate);
int32_t nrf24_set_data_rate(nrf24_handle handle, const uint8_t data_rate);

int32_t nrf24_get_power(nrf24_handle handle, uint8_t *power);
int32_t nrf24_set_power(nrf24_handle handle, const uint8_t power);

int32_t nrf24_get_rx_payload_length(nrf24_handle handle, const uint8_t pipe, uint8_t *length);
int32_t nrf24_set_rx_payload_length(nrf24_handle handle, const uint8_t pipe, const uint8_t length);

int32_t nrf24_clear_status(nrf24_handle handle);

int32_t nrf24_get_rx_address(nrf24_handle handle, const uint8_t pipe, uint8_t *address, const uint32_t address_len);
int32_t nrf24_set_rx_address(nrf24_handle handle, const uint8_t pipe, const uint8_t *address, const uint32_t address_len);
int32_t nrf24_set_rx_address_byte(nrf24_handle handle, const uint8_t pipe, const uint8_t address);

int32_t nrf24_get_tx_address(nrf24_handle handle, uint8_t *address, const uint32_t address_len);
int32_t nrf24_set_tx_address(nrf24_handle handle, const uint8_t *address, const uint32_t address_len);

int32_t nrf24_write_tx_payload(nrf24_handle handle, const uint8_t *data, const uint8_t len);
int32_t nrf24_read_rx_payload(nrf24_handle handle, uint8_t *data, const uint8_t len);

int32_t nrf24_flush_rx(nrf24_handle handle);
int32_t nrf24_flush_tx(nrf24_handle handle);

int32_t nrf24_start_listen(nrf24_handle handle);
int32_t nrf24_stop_listen(nrf24_handle handle);

int32_t nrf24_get_status(nrf24_handle handle, uint8_t *data_ready, uint8_t *data_sent, uint8_t *max_retry);

int32_t nrf24_send(nrf24_handle handle, const uint8_t *data, const uint8_t len);
int32_t nrf24_receive(nrf24_handle handle, uint8_t *data, const uint8_t len);

#endif /* NRF24_H */
