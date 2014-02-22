/* 
 * Copyright (c) 2013-2014, Erik Svensson <erik.public@gmail.com>
 *
 * Decoding of byte streams
 */

#ifndef NRF24_DECODE_H
#define NRF24_DECODE_H

#include <stdint.h>

/* Decode value from bytestream
 *
 * returns a bytes used.
 */
int32_t nrf24_decode_int8(int8_t *value, const uint8_t *bytes, const int32_t len);
int32_t nrf24_decode_uint8(uint8_t *value, const uint8_t *bytes, const int32_t len);
int32_t nrf24_decode_int16(int16_t *value, const uint8_t *bytes, const int32_t len);
int32_t nrf24_decode_uint16(uint16_t *value, const uint8_t *bytes, const int32_t len);
int32_t nrf24_decode_int32(int32_t *value, const uint8_t *bytes, const int32_t len);
int32_t nrf24_decode_uint32(uint32_t *value, const uint8_t *bytes, const int32_t len);
int32_t nrf24_decode_int64(int64_t *value, const uint8_t *bytes, const int32_t len);
int32_t nrf24_decode_uint64(uint64_t *value, const uint8_t *bytes, const int32_t len);

#endif /* NRF24_DECODE_H */
