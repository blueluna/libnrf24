/* 
 * Copyright (c) 2013-2014, Erik Svensson <erik.public@gmail.com>
 *
 * Handling of SPI through linux spi devices (/dev/spidev)
 */

#include "spi.h"
#include "errorcodes.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

int32_t nrf24_spi_open(const uint32_t controller, const uint32_t device, const uint32_t speed, const uint8_t bits
	, const uint8_t mode)
{
	int32_t fd = -1;
	int32_t result = NRF24_OK;
	char devicefile[256] = {0};
	if (sprintf(devicefile, "/dev/spidev%d.%d", controller, device) >= 0) {
		fd = open(devicefile, O_RDWR);
	}
	if (fd < 0) {
		fd = -1;
		result = NRF24_OPEN_FAILED;
	}
	/*
	 * spi mode
	 */
	if (result == 0) {
		result = ioctl(fd, SPI_IOC_WR_MODE, &mode);
		if (result == -1) {
			result = NRF24_SETUP_FAILED;
			perror("can't set spi write mode");
		}
	}
	if (result == 0) {
		result = ioctl(fd, SPI_IOC_RD_MODE, &mode);
		if (result == -1) {
			result = NRF24_SETUP_FAILED;
			perror("can't set spi read mode");
		}
	}
	/*
	 * bits per word
	 */
	if (result == 0) {
		result = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
		if (result == -1) {
			result = NRF24_SETUP_FAILED;
			perror("can't set write bits per word");
		}
	}
	if (result == 0) {
		result = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
		if (result == -1) {
			result = NRF24_SETUP_FAILED;
			perror("can't set read bits per word");
		}
	}
	/*
	 * max speed hz
	 */
	if (result == 0) {
		result = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
		if (result == -1) {
			result = NRF24_SETUP_FAILED;
			perror("can't set write max speed hz");
		}
	}
	if (result == 0) {
		result = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
		if (result == -1) {
			result = NRF24_SETUP_FAILED;
			perror("can't set read max speed hz");
		}
	}
	if (result < 0) {
		if (fd >= 0) {
			close(fd);
		}
		return result;
	}
	return fd;
}

int32_t nrf24_spi_close(const int32_t handle)
{
	if (handle >= 0) {
		close(handle);
		return NRF24_OK;
	}
	return NRF24_INVALID_HANDLE;
}

int32_t nrf24_spi_transfer(const int32_t handle, uint8_t *tx, uint8_t *rx, const uint16_t len)
{
	int32_t result = NRF24_OK;
	struct spi_ioc_transfer tr = {
		.tx_buf = 0,
		.rx_buf = 0,
		.len = 0,
		.delay_usecs = 0,
		.cs_change = 0
	};

	if (handle < 0) {
		return NRF24_INVALID_HANDLE;
	}
	if (tx == 0 || rx == 0) {
		return NRF24_INVALID_ARGUMENT;
	}

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = len;
	tr.delay_usecs = 0;
	tr.cs_change = 1;

	result = ioctl(handle, SPI_IOC_MESSAGE(1), &tr);
	if (result < 1) {
		result = NRF24_TRANSFER_FAILED;
	}
	return result;
}
