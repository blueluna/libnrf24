/* 
 * Copyright (c) 2013-2014, Erik Svensson <erik.public@gmail.com>
 *
 * Constants for use with nrf24xxxx handling
 */

#ifndef NRF24_CONSTANTS_H
#define NRF24_CONSTANTS_H

/* SPI commands */
#define NRF24_SPI_READ_REGISTER 0x00 /* Address mask == 0x1F */
#define NRF24_SPI_WRITE_REGISTER 0x20 /* Address mask == 0x1F */
#define NRF24_SPI_READ_RX_PAYLOAD 0x61
#define NRF24_SPI_WRITE_TX_PAYLOAD 0xA0
#define NRF24_SPI_FLUSH_TX 0xE1
#define NRF24_SPI_FLUSH_RX 0xE2
#define NRF24_SPI_REUSE_TX_PAYLOAD 0xE3
#define NRF24_SPI_READ_RX_PAYLOAD_WIDTH 0x60
#define NRF24_SPI_WRITE_ACK_PAYLOAD 0xA1 /* Response pipe mask == 0x07 */
#define NRF24_SPI_WRITE_TX_PAYLOAD_NO_ACK 0xB0
#define NRF24_SPI_NOP 0xFF

/* SPI registers */
#define NRF24_REG_CONFIG 0x00
#define NRF24_REG_EN_AA 0x01
#define NRF24_REG_EN_RX_ADDR 0x02
#define NRF24_REG_SETUP_AW 0x03
#define NRF24_REG_SETUP_RETR 0x04
#define NRF24_REG_RF_CH 0x05
#define NRF24_REG_RF_SETUP 0x06
#define NRF24_REG_STATUS 0x07
#define NRF24_REG_OBSERVE_TX 0x08
#define NRF24_REG_RPD 0x09
#define NRF24_REG_RX_ADDR_P0 0x0A
#define NRF24_REG_RX_ADDR_P1 0x0B
#define NRF24_REG_RX_ADDR_P2 0x0C
#define NRF24_REG_RX_ADDR_P3 0x0D
#define NRF24_REG_RX_ADDR_P4 0x0E
#define NRF24_REG_RX_ADDR_P5 0x0F
#define NRF24_REG_TX_ADDR 0x10
#define NRF24_REG_RX_PW_P0 0x11
#define NRF24_REG_RX_PW_P1 0x12
#define NRF24_REG_RX_PW_P2 0x13
#define NRF24_REG_RX_PW_P3 0x14
#define NRF24_REG_RX_PW_P4 0x15
#define NRF24_REG_RX_PW_P5 0x16
#define NRF24_REG_FIFO_STATUS 0x17
#define NRF24_REG_DYNPD 0x1C
#define NRF24_REG_FEATURE 0x1D

#define NRF24_REGISTER_ADDRESS_MASK 0x1F

/* Radio data rates */
#define NRF24_DATA_RATE_1MBPS 0x00
#define NRF24_DATA_RATE_2MBPS 0x08
#define NRF24_DATA_RATE_250KBPS 0x20

/* Radio Power levels */
#define NRF24_POWER_N18DBM 0x00
#define NRF24_POWER_N12DBM 0x02
#define NRF24_POWER_N6DBM 0x04
#define NRF24_POWER_0DBM 0x06

/* Configuration constants */
#define NRF24_CONFIG_PWR_UP 0x02
#define NRF24_CONFIG_PRIM_RX 0x01

/* Status constants */
#define NRF24_STATUS_RX_DR 0x40
#define NRF24_STATUS_TX_DS 0x20
#define NRF24_STATUS_MAX_RT 0x10
#define NRF24_STATUS_TX_FIFO_FULL 0x01

#endif /* NRF24_CONSTANTS_H */
