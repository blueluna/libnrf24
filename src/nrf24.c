#include "nrf24.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "spi.h"
#include "gpio.h"

int32_t nrf24_ce(nrf24_handle handle, const uint8_t level)
{
  return nrf24_gpio_write(handle->ce_pin, level);
}

nrf24_handle nrf24_open(const int32_t spi_handle, const uint16_t ce_pin)
{
  int32_t result = 0;
  nrf24_handle handle = 0;
  if (spi_handle < 0) {
    return 0;
  }
  handle = malloc(sizeof(nrf24_ctx_t));
  if (handle != 0) {
    handle->rx_buf = malloc(33);
    if (handle->rx_buf == 0) {
      free(handle);
      return 0;
    }
    memset(handle->rx_buf, 0, 33);
    handle->tx_buf = malloc(33);
    if (handle->tx_buf == 0) {
      free(handle->rx_buf);
      free(handle);
      return 0;
    }
    memset(handle->tx_buf, 0xff, 33);
    result = nrf24_gpio_open(ce_pin, NRF24_GPIO_OUTPUT);
    if (result != 0) {
      free(handle->tx_buf);
      free(handle->rx_buf);
      free(handle);
      return 0;
    }
    handle->ce_pin = ce_pin;
    nrf24_gpio_write(handle->ce_pin, NRF24_GPIO_LOW);
    handle->spi_handle = spi_handle;
    return handle;
  }
  return 0;
}

int32_t nrf24_close(nrf24_handle handle)
{
  if (handle != 0) {
    nrf24_gpio_close(handle->ce_pin);
    free(handle->tx_buf);
    free(handle->rx_buf);
    free(handle);
  }
  return 0;
}

int32_t nrf24_get_register(nrf24_handle handle, const uint8_t address, uint8_t *value)
{
  int32_t result = 0;
  if (handle == 0 || address > NRF24_REGISTER_ADDRESS_MASK || value == 0) {
    return -1;
  }
  handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | (address & NRF24_REGISTER_ADDRESS_MASK);
  handle->tx_buf[1] = NRF24_SPI_NOP;
  result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 2);
  if (result >= 0) {
    *value = handle->rx_buf[1];
  }
  return result;
}

int32_t nrf24_set_register(nrf24_handle handle, const uint8_t address, const uint8_t value)
{
  if (handle == 0 || address > NRF24_REGISTER_ADDRESS_MASK) {
    return -1;
  }
  handle->tx_buf[0] = NRF24_SPI_WRITE_REGISTER | (address & NRF24_REGISTER_ADDRESS_MASK);
  handle->tx_buf[1] = value;
  return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, 0, 2);
}

int32_t nrf24_get_rx_address(nrf24_handle handle, const uint8_t pipe, uint64_t *address)
{
  int32_t result = 0;
  uint64_t addr = 0;
  if (handle == 0 || pipe > 5 || address == 0) {
    return -1;
  }
  memset(handle->tx_buf, 0xff, 6);
  if (pipe > 1) {
    handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | NRF24_REG_RX_ADDR_P1; /* Read MSB */
    result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
    if (result >= 0) {
      addr = ((uint64_t)(handle->rx_buf[5]) << 32) | ((uint64_t)(handle->rx_buf[4]) << 24) | ((uint64_t)(handle->rx_buf[3]) << 16) | ((uint64_t)(handle->rx_buf[2]) << 8);
      handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | (NRF24_REG_RX_ADDR_P0 + pipe); /* Read LSB */
      result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 2);
      if (result >= 0) {
        addr |= (uint64_t)(handle->rx_buf[1]);
      }
    }
  }
  else {
    handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | (NRF24_REG_RX_ADDR_P0 + pipe);
    result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
    if (result >= 0) {
      addr = ((uint64_t)(handle->rx_buf[5]) << 32) | ((uint64_t)(handle->rx_buf[4]) << 24) | ((uint64_t)(handle->rx_buf[3]) << 16) | ((uint64_t)(handle->rx_buf[2]) << 8) | (uint64_t)(handle->rx_buf[1]);
    }
  }
  if (result >= 0) {
    *address = addr;
  }
  return result;
}

int32_t nrf24_set_rx_address(nrf24_handle handle, const uint8_t pipe, const uint64_t address)
{
  if (handle == 0 || pipe > 1) {
    return -1;
  }
  handle->tx_buf[0] = NRF24_SPI_WRITE_REGISTER | (NRF24_REG_RX_ADDR_P0 + pipe);
  handle->tx_buf[1] = (uint8_t)(address);
  handle->tx_buf[2] = (uint8_t)(address >> 8);
  handle->tx_buf[3] = (uint8_t)(address >> 16);
  handle->tx_buf[4] = (uint8_t)(address >> 24);
  handle->tx_buf[5] = (uint8_t)(address >> 32);
  return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, 0, 6);
}

int32_t nrf24_set_rx_address_byte(nrf24_handle handle, const uint8_t pipe, const uint8_t address)
{
  if (pipe < 2 && pipe > 5) {
    return -1;
  }
  return nrf24_set_register(handle, (NRF24_REG_RX_ADDR_P0 + pipe), address);
}

int32_t nrf24_get_tx_address(nrf24_handle handle, uint64_t *address)
{
  int32_t result = 0;
  if (handle == 0 || address == 0) {
    return -1;
  }
  memset(handle->tx_buf, 0xff, 6);
  handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | NRF24_REG_TX_ADDR;
  result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
  if (result >= 0) {
    *address = ((uint64_t)(handle->rx_buf[5]) << 32) | ((uint64_t)(handle->rx_buf[4]) << 24) | ((uint64_t)(handle->rx_buf[3]) << 16) | ((uint64_t)(handle->rx_buf[2]) << 8) | (uint64_t)(handle->rx_buf[1]);
  }
  return result;
}

int32_t nrf24_set_tx_address(nrf24_handle handle, const uint64_t address)
{
  handle->tx_buf[0] = NRF24_SPI_WRITE_REGISTER | NRF24_REG_TX_ADDR;
  handle->tx_buf[1] = (uint8_t)(address);
  handle->tx_buf[2] = (uint8_t)(address >> 8);
  handle->tx_buf[3] = (uint8_t)(address >> 16);
  handle->tx_buf[4] = (uint8_t)(address >> 24);
  handle->tx_buf[5] = (uint8_t)(address >> 32);
  return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, 0, 6);
}

int32_t nrf24_powerup(nrf24_handle handle)
{
  uint8_t reg = 0;
  if (nrf24_get_register(handle, NRF24_REG_CONFIG, &reg) >= 0) {
    if ((reg & 0x02) == 0x02) {
      return 0;
    }
    else {
      reg |= 0x02;
      return nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
    }
  }
  return -1;
}

int32_t nrf24_powerdown(nrf24_handle handle)
{
  uint8_t reg = 0;
  if (nrf24_get_register(handle, NRF24_REG_CONFIG, &reg) >= 0) {
    if ((reg & 0x02) == 0x02) {
      reg &= 0xFD;
      return nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
    }
    else {
      return 0;
    }
  }
  return -1;
}

int32_t nrf24_get_crc(nrf24_handle handle, uint8_t *bytes)
{
  uint8_t reg = 0;
  if (bytes == 0) {
    return -1;
  }
  if (nrf24_get_register(handle, NRF24_REG_CONFIG, &reg) >= 0) {
    if ((reg & 0x08) == 0x08) {
      if ((reg & 0x04) == 0x04) {
	*bytes = 2;
      }
      else {
	*bytes = 1;
      }
    }
    else {
      *bytes = 0;
    }
    return 0;
  }
  return -1;
}

int32_t nrf24_set_crc(nrf24_handle handle, const uint8_t bytes)
{
  uint8_t reg = 0;
  if (nrf24_get_register(handle, NRF24_REG_CONFIG, &reg) >= 0) {    
    uint8_t crc = 0;
    if (bytes == 1) { crc = 0x08; }
    else if (bytes == 2) { crc = 0x0C; }
    reg = (reg & 0xF3) | crc;
    return nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
  }
  return -1;
}

int32_t nrf24_get_channel(nrf24_handle handle, uint8_t *channel)
{
  if (channel == 0) {
    return -1;
  }
  if (nrf24_get_register(handle, NRF24_REG_RF_CH, channel) >= 0) {
    return 0;
  }
  return -1;
}

int32_t nrf24_set_channel(nrf24_handle handle, const uint8_t channel)
{
  if (channel <= 0x7f) {
    return nrf24_set_register(handle, NRF24_REG_RF_CH, channel);
  }
  return -1;
}

int32_t nrf24_get_data_rate(nrf24_handle handle, uint8_t *data_rate)
{
  uint8_t reg = 0;
  if (data_rate == 0) {
    return -1;
  }
  if (nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg) >= 0) {
    if ((reg & 0x20) == 0x20) {
      *data_rate = NRF24_DATA_RATE_250KBPS;
    }
    else {
      if ((reg & 0x08) == 0x08) {
	*data_rate = NRF24_DATA_RATE_2MBPS;
      }
      else {
	*data_rate = NRF24_DATA_RATE_1MBPS;
      }
    }
    return 0;
  }
  return -1;
}

int32_t nrf24_set_data_rate(nrf24_handle handle, const uint8_t data_rate)
{
  uint8_t reg = 0;
  if (nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg) >= 0) {
    uint8_t dr = 0;
    if (data_rate == NRF24_DATA_RATE_1MBPS) {
      dr = 0x08;
    }
    else if (data_rate == NRF24_DATA_RATE_2MBPS) {
      dr = 0x28;
    }
    else {
      dr = 0x20;
    }
    reg = (reg & 0xD7) | dr;
    return nrf24_set_register(handle, NRF24_REG_RF_SETUP, reg);
  }
  return -1;
}

int32_t nrf24_get_power(nrf24_handle handle, uint8_t *power)
{
  uint8_t reg = 0;
  if (power == 0) {
    return -1;
  }
  if (nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg) >= 0) {
    *power = (reg & 0x06);
    return 0;
  }
  return -1;
}

int32_t nrf24_set_power(nrf24_handle handle, const uint8_t power)
{
  uint8_t reg = 0;
  if (nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg) >= 0) {
    reg = (reg & 0xF9) | power;
    return nrf24_set_register(handle, NRF24_REG_RF_SETUP, reg);
  }
  return -1;
}

int32_t nrf24_get_rx_payload_length(nrf24_handle handle, const uint8_t pipe, uint8_t *length)
{
  if (pipe > 5) {
    return -1;
  }
  return nrf24_get_register(handle, NRF24_REG_RX_PW_P0 + pipe, length);
}

int32_t nrf24_set_rx_payload_length(nrf24_handle handle, const uint8_t pipe, const uint8_t length)
{
  if (pipe > 5 || length > 32) {
    return -1;
  }
  return nrf24_set_register(handle, NRF24_REG_RX_PW_P0 + pipe, length);
}

int32_t nrf24_clear_status(nrf24_handle handle)
{
  return nrf24_set_register(handle, NRF24_REG_STATUS, NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
}

int32_t nrf24_write_tx_payload(nrf24_handle handle, const uint8_t *data, const uint8_t len)
{
  if (handle == 0 || data == 0 || len == 0 || len > 32) {
    return -1;
  }
  memcpy(handle->tx_buf + 1, data, len);
  handle->tx_buf[0] = NRF24_SPI_WRITE_TX_PAYLOAD;
  if (nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, (len + 1)) >= 0) {
    return len;
  }
  else {
    return -1;
  }
}

int32_t nrf24_read_rx_payload(nrf24_handle handle, uint8_t *data, const uint8_t len)
{
  int32_t result;
  if (handle == 0 || data == 0 || len == 0 || len > 32) {
    return -1;
  }
  memset(handle->tx_buf, 0xff, (len+1));
  handle->tx_buf[0] = NRF24_SPI_READ_RX_PAYLOAD;
  result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, (len + 1));
  if (result >= 0) {
    memcpy(data, handle->rx_buf + 1, len);
    return len;
  }
  return -1;
}

int32_t nrf24_flush_rx(nrf24_handle handle)
{
  handle->tx_buf[0] = NRF24_SPI_FLUSH_RX;
  return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, 0, 1);
}

int32_t nrf24_flush_tx(nrf24_handle handle)
{
  handle->tx_buf[0] = NRF24_SPI_FLUSH_TX;
  return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, 0, 1);
}

int32_t nrf24_listen(nrf24_handle handle)
{
  uint8_t reg;
  int32_t result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
  if (result >= 0) {
    reg |= (NRF24_CONFIG_PWR_UP | NRF24_CONFIG_PRIM_RX);
    result = nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
  }
  if (result >= 0) {
    result = nrf24_clear_status(handle);
  }
  if (result >= 0) {
    result = nrf24_ce(handle, NRF24_GPIO_HIGH);
  }
  return result;
}

int32_t nrf24_stop_listen(nrf24_handle handle)
{
  int32_t result = nrf24_ce(handle, NRF24_GPIO_LOW);
  nrf24_flush_rx(handle);
  nrf24_flush_tx(handle);
  return result;
}

int32_t nrf24_get_status(nrf24_handle handle, uint8_t *data_ready, uint8_t *data_sent, uint8_t *max_retry)
{
  uint8_t reg;
  int32_t result;
  result = nrf24_get_register(handle, NRF24_REG_STATUS, &reg);
  if (result >= 0) {
    if (data_ready != 0) {
      *data_ready = (reg & NRF24_STATUS_RX_DR) == NRF24_STATUS_RX_DR;
    }
    if (data_sent != 0) {
      *data_sent = (reg & NRF24_STATUS_TX_DS) == NRF24_STATUS_TX_DS;
    }
    if (max_retry != 0) {
      *max_retry = (reg & NRF24_STATUS_MAX_RT) == NRF24_STATUS_MAX_RT;
    }
  }
  return result;
}

int32_t nrf24_send(nrf24_handle handle, const uint8_t *data, const uint8_t len)
{
  int32_t result;
  uint8_t reg = 0;
  if (data == 0 || len == 0) {
    return -1;
  }
  result = nrf24_ce(handle, NRF24_GPIO_LOW);
  if (result >= 0) {
    result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
  }
  if (result >= 0) {
    reg = (reg & 0xFC) | NRF24_CONFIG_PWR_UP;
    result = nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
  }
  if (result >= 0) {
    result = nrf24_write_tx_payload(handle, data, len);
  }
  if (result >= 0) {
    result = nrf24_clear_status(handle);
  }
  result = nrf24_ce(handle, NRF24_GPIO_HIGH);
  /* Wait for data send or max retries */
  do {
    result = nrf24_get_register(handle, NRF24_REG_STATUS, &reg);
  } while ((reg & NRF24_STATUS_TX_DS) == NRF24_STATUS_TX_DS || (reg & NRF24_STATUS_MAX_RT) == NRF24_STATUS_MAX_RT);
  /* Clear status */
  if ((reg & NRF24_STATUS_TX_DS) == NRF24_STATUS_TX_DS) {
    nrf24_set_register(handle, NRF24_REG_STATUS, NRF24_STATUS_TX_DS);
    result = 0;
  }
  if ((reg & NRF24_STATUS_MAX_RT) == NRF24_STATUS_MAX_RT) {
    nrf24_set_register(handle, NRF24_REG_STATUS, NRF24_STATUS_MAX_RT);
    result = -2;
  }
  result = nrf24_ce(handle, NRF24_GPIO_LOW);
  return result;
}

int32_t nrf24_receive(nrf24_handle handle, uint8_t *data, const uint8_t len)
{
  uint8_t reg;
  int32_t result;
  if (data == 0 || len == 0) {
    return -1;
  }
  result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
  if (result >= 0) {
    if ((reg & NRF24_CONFIG_PRIM_RX) == 0) {
      result = -1;
    }
  }
  if (result >= 0) {
    /* Check status */
    result = nrf24_get_register(handle, NRF24_REG_STATUS, &reg);
  }
  if (result >= 0) {
    if ((reg & NRF24_STATUS_RX_DR) == 0) {
      result = -1;
    }
  }
  if (result >= 0) {
    result = nrf24_read_rx_payload(handle, data, len);
  }
  /* Clear status */
  nrf24_set_register(handle, NRF24_REG_STATUS, NRF24_STATUS_RX_DR);
  return result;  
}
