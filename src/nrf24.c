#include "nrf24.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "spi.h"
#include "gpio.h"
#include "errorcodes.h"
#include "sleep.h"

int32_t nrf24_ce(nrf24_handle handle, const uint8_t level)
{
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
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
		return NRF24_OK;
	}
	return NRF24_INVALID_HANDLE;
}

int32_t nrf24_get_register(nrf24_handle handle, const uint8_t address, uint8_t *value)
{
	int32_t result = 0;
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (address > NRF24_REGISTER_ADDRESS_MASK || value == 0) {
		return NRF24_INVALID_ARGUMENT;
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
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (address > NRF24_REGISTER_ADDRESS_MASK) {
		return NRF24_INVALID_ARGUMENT;
	}
	handle->tx_buf[0] = NRF24_SPI_WRITE_REGISTER | (address & NRF24_REGISTER_ADDRESS_MASK);
	handle->tx_buf[1] = value;
	return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 2);
}

int32_t nrf24_get_rx_address(nrf24_handle handle, const uint8_t pipe, uint8_t *address, const uint32_t address_len)
{
	int32_t result = NRF24_OK;
	uint64_t addr = 0;
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (pipe > 5 || address == 0 || address_len < 5) {
		return NRF24_INVALID_ARGUMENT;
	}
	memset(handle->tx_buf, 0xff, 6);
	if (pipe > 1) {
		handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | NRF24_REG_RX_ADDR_P1; /* Read MSB */
		result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
		if (result >= 0) {
			address[1] = handle->rx_buf[2];
			address[2] = handle->rx_buf[3];
			address[3] = handle->rx_buf[4];
			address[4] = handle->rx_buf[5];
			handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | (NRF24_REG_RX_ADDR_P0 + pipe); /* Read LSB */
			result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 2);
			if (result >= 0) {
				address[0] = handle->rx_buf[1];
			}
		}
	}
	else {
		handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | (NRF24_REG_RX_ADDR_P0 + pipe);
		result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
		if (result >= 0) {
			address[0] = handle->rx_buf[1];
			address[1] = handle->rx_buf[2];
			address[2] = handle->rx_buf[3];
			address[3] = handle->rx_buf[4];
			address[4] = handle->rx_buf[5];
		}
	}
	if (result >= 0) {
		*address = addr;
	}
	return result;
}

int32_t nrf24_set_rx_address(nrf24_handle handle, const uint8_t pipe, const uint8_t *address, const uint32_t address_len)
{
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (pipe > 1 || address == 0 || address_len < 5) {
		return NRF24_INVALID_ARGUMENT;
	}
	handle->tx_buf[0] = NRF24_SPI_WRITE_REGISTER | (NRF24_REG_RX_ADDR_P0 + pipe);
	handle->tx_buf[1] = address[0];
	handle->tx_buf[2] = address[1];
	handle->tx_buf[3] = address[2];
	handle->tx_buf[4] = address[3];
	handle->tx_buf[5] = address[4];
	return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
}

int32_t nrf24_set_rx_address_byte(nrf24_handle handle, const uint8_t pipe, const uint8_t address)
{
	if (pipe < 2 && pipe > 5) {
		return NRF24_INVALID_ARGUMENT;
	}
	return nrf24_set_register(handle, (NRF24_REG_RX_ADDR_P0 + pipe), address);
}

int32_t nrf24_get_tx_address(nrf24_handle handle, uint8_t *address, const uint32_t address_len)
{
	int32_t result = NRF24_OK;
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (address == 0 || address_len < 5) {
		return NRF24_INVALID_ARGUMENT;
	}
	memset(handle->tx_buf, 0xff, 6);
	handle->tx_buf[0] = NRF24_SPI_READ_REGISTER | NRF24_REG_TX_ADDR;
	result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
	if (result >= 0) {
			address[0] = handle->rx_buf[1];
			address[1] = handle->rx_buf[2];
			address[2] = handle->rx_buf[3];
			address[3] = handle->rx_buf[4];
			address[4] = handle->rx_buf[5];
	}
	return result;
}

int32_t nrf24_set_tx_address(nrf24_handle handle, const uint8_t *address, const uint32_t address_len)
{
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (address == 0 || address_len < 5) {
		return NRF24_INVALID_ARGUMENT;
	}
	handle->tx_buf[0] = NRF24_SPI_WRITE_REGISTER | NRF24_REG_TX_ADDR;
	handle->tx_buf[1] = address[0];
	handle->tx_buf[2] = address[1];
	handle->tx_buf[3] = address[2];
	handle->tx_buf[4] = address[3];
	handle->tx_buf[5] = address[4];
	return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 6);
}

int32_t nrf24_powerup(nrf24_handle handle)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
	if (result >= 0) {
		if ((reg & 0x02) == 0x02) {
			return NRF24_OK;
		}
		else {
			reg |= 0x02;
			return nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
		}
	}
	return result;
}

int32_t nrf24_powerdown(nrf24_handle handle)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
	if (result >= 0) {
		if ((reg & 0x02) == 0x02) {
			reg &= 0xFD;
			return nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
		}
		else {
			return NRF24_OK;
		}
	}
	return result;
}

int32_t nrf24_get_crc(nrf24_handle handle, uint8_t *bytes)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	if (bytes == 0) {
		return NRF24_INVALID_ARGUMENT;
	}
	result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
	if (result >= 0) {
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
		return NRF24_OK;
	}
	return result;
}

int32_t nrf24_set_crc(nrf24_handle handle, const uint8_t bytes)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
	if (result >= 0) {    
		uint8_t crc = 0;
		if (bytes == 1) { crc = 0x08; }
		else if (bytes == 2) { crc = 0x0C; }
		reg = (reg & 0xF3) | crc;
		return nrf24_set_register(handle, NRF24_REG_CONFIG, reg);
	}
	return result;
}

int32_t nrf24_get_channel(nrf24_handle handle, uint8_t *channel)
{
	if (channel == 0) {
		return NRF24_INVALID_ARGUMENT;
	}
	return nrf24_get_register(handle, NRF24_REG_RF_CH, channel);
}

int32_t nrf24_set_channel(nrf24_handle handle, const uint8_t channel)
{
	if (channel <= 0x7f) {
		return nrf24_set_register(handle, NRF24_REG_RF_CH, channel);
	}
	return NRF24_INVALID_ARGUMENT;
}

int32_t nrf24_get_data_rate(nrf24_handle handle, uint8_t *data_rate)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	if (data_rate == 0) {
		return NRF24_INVALID_ARGUMENT;
	}
	result = nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg);
	if (result >= 0) {
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
		return NRF24_OK;
	}
	return result;
}

int32_t nrf24_set_data_rate(nrf24_handle handle, const uint8_t data_rate)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	result = nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg);
	if (result >= 0) {
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
	return result;
}

int32_t nrf24_get_power(nrf24_handle handle, uint8_t *power)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	if (power == 0) {
		return NRF24_INVALID_ARGUMENT;
	}
	result = nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg);
	if (result >= 0) {
		*power = (reg & 0x06);
		return NRF24_OK;
	}
	return result;
}

int32_t nrf24_set_power(nrf24_handle handle, const uint8_t power)
{
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	result = nrf24_get_register(handle, NRF24_REG_RF_SETUP, &reg);
	if (result >= 0) {
		reg = (reg & 0xF9) | power;
		return nrf24_set_register(handle, NRF24_REG_RF_SETUP, reg);
	}
	return result;
}

int32_t nrf24_get_rx_payload_length(nrf24_handle handle, const uint8_t pipe, uint8_t *length)
{
	if (pipe > 5) {
		return NRF24_INVALID_ARGUMENT;
	}
	return nrf24_get_register(handle, NRF24_REG_RX_PW_P0 + pipe, length);
}

int32_t nrf24_set_rx_payload_length(nrf24_handle handle, const uint8_t pipe, const uint8_t length)
{
	if (pipe > 5 || length > 32) {
		return NRF24_INVALID_ARGUMENT;
	}
	return nrf24_set_register(handle, NRF24_REG_RX_PW_P0 + pipe, length);
}

int32_t nrf24_get_auto_retransmit(nrf24_handle handle, uint8_t *retries, uint16_t *delay)
{
	uint8_t regval = 0;
	int32_t result = NRF24_OK;
	if (delay == 0 && retries == 0) {
		return NRF24_INVALID_ARGUMENT;
	}
	result = nrf24_get_register(handle, NRF24_REG_SETUP_RETR, &regval);
	if (result == NRF24_OK) {
		if (delay != 0) {
			*delay = ((regval >> 4) + 1) * 250;
		}
		if (retries != 0) {
			*retries = (regval & 0x0f);
		}
	}
	return result;
}

int32_t nrf24_set_auto_retransmit(nrf24_handle handle, const uint8_t retries, const uint16_t delay)
{
	uint8_t regval = 0;
	if (delay < 250 || delay > 4000 || retries > 15) {
		return NRF24_INVALID_ARGUMENT;
	}
	regval = (((uint8_t)(delay / 250)) << 4) | retries;
	return nrf24_set_register(handle, NRF24_REG_SETUP_RETR, regval);
}

int32_t nrf24_clear_status(nrf24_handle handle)
{
	return nrf24_set_register(handle, NRF24_REG_STATUS, NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
}

int32_t nrf24_write_tx_payload(nrf24_handle handle, const uint8_t *data, const uint8_t len)
{
	int32_t result = NRF24_OK;
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (data == 0 || len == 0 || len > 32) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(handle->tx_buf + 1, data, len);
	handle->tx_buf[0] = NRF24_SPI_WRITE_TX_PAYLOAD;
	result = nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, (len + 1));
	if (result >= 0) {
		return len;
	}
	else {
		return result;
	}
}

int32_t nrf24_read_rx_payload(nrf24_handle handle, uint8_t *data, const uint8_t len)
{
	int32_t result = NRF24_OK;
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (data == 0 || len == 0 || len > 32) {
		return NRF24_INVALID_ARGUMENT;
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
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	handle->tx_buf[0] = NRF24_SPI_FLUSH_RX;
	return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 1);
}

int32_t nrf24_flush_tx(nrf24_handle handle)
{
	if (handle == 0) {
		return NRF24_INVALID_HANDLE;
	}
	handle->tx_buf[0] = NRF24_SPI_FLUSH_TX;
	return nrf24_spi_transfer(handle->spi_handle, handle->tx_buf, handle->rx_buf, 1);
}

int32_t nrf24_start_listen(nrf24_handle handle)
{
	int32_t result = NRF24_OK;
	uint8_t reg;
	result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
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
	int32_t result = NRF24_OK;
	result = nrf24_ce(handle, NRF24_GPIO_LOW);
	if (result >= 0) {
		result = nrf24_flush_rx(handle);
	}
	if (result >= 0) {
		result = nrf24_flush_tx(handle);
	}
	return result;
}

int32_t nrf24_get_status(nrf24_handle handle, uint8_t *data_ready, uint8_t *data_sent, uint8_t *max_retry)
{
	int32_t result = NRF24_OK;
	uint8_t reg;
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
	int32_t result = NRF24_OK;
	uint8_t reg = 0;
	if (data == 0 || len == 0) {
		return NRF24_INVALID_ARGUMENT;
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
		result = NRF24_OK;
	}
	if ((reg & NRF24_STATUS_MAX_RT) == NRF24_STATUS_MAX_RT) {
		nrf24_set_register(handle, NRF24_REG_STATUS, NRF24_STATUS_MAX_RT);
		result = NRF24_MAX_RETRIES;
	}
	result = nrf24_ce(handle, NRF24_GPIO_LOW);
	return result;
}

int32_t nrf24_receive(nrf24_handle handle, uint8_t *data, const uint8_t len)
{
	int32_t result = NRF24_OK;
	uint8_t reg;
	if (data == 0 || len == 0) {
		return NRF24_INVALID_ARGUMENT;
	}
	result = nrf24_get_register(handle, NRF24_REG_CONFIG, &reg);
	if (result >= 0) {
		if ((reg & NRF24_CONFIG_PRIM_RX) == 0) {
			result = NRF24_NOT_LISTENING;
		}
	}
	if (result >= 0) {
		/* Check status */
		result = nrf24_get_register(handle, NRF24_REG_STATUS, &reg);
	}
	if (result >= 0) {
		if ((reg & NRF24_STATUS_RX_DR) == 0) {
			result = NRF24_NO_DATA;
		}
	}
	if (result >= 0) {
		result = nrf24_read_rx_payload(handle, data, len);
	}
	/* Clear status */
	nrf24_set_register(handle, NRF24_REG_STATUS, NRF24_STATUS_RX_DR);
	return result;  
}
