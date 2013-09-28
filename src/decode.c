/* 
 * Copyright (c) 2013, Erik Svensson <erik.public@gmail.com>
 *
 * Decoding of byte streams
 */

#include "decode.h"
#include "errorcodes.h"
#include <string.h>


int32_t nrf24_decode_int8(int8_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(int8_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(int8_t));
	return sizeof(int8_t);
}

int32_t nrf24_decode_uint8(uint8_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(uint8_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(uint8_t));
	return sizeof(uint8_t);
}

int32_t nrf24_decode_int16(int16_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(int16_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(int16_t));
	return sizeof(int16_t);
}

int32_t nrf24_decode_uint16(uint16_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(uint16_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(uint16_t));
	return sizeof(uint16_t);
}

int32_t nrf24_decode_int32(int32_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(int32_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(int32_t));
	return sizeof(int32_t);
}

int32_t nrf24_decode_uint32(uint32_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(uint32_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(uint32_t));
	return sizeof(uint32_t);
}

int32_t nrf24_decode_int64(int64_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(int64_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(int64_t));
	return sizeof(int64_t);
}

int32_t nrf24_decode_uint64(uint64_t *value, const uint8_t *bytes, const int32_t len)
{
	if (value == 0 || bytes == 0 || len < sizeof(uint64_t)) {
		return NRF24_INVALID_ARGUMENT;
	}
	memcpy(value, bytes, sizeof(uint64_t));
	return sizeof(uint64_t);
}
