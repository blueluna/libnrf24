/* 
 * Copyright (c) 2013-2014, Erik Svensson <erik.public@gmail.com>
 */

#ifndef NRF24_ERRORCCODES_H
#define NRF24_ERRORCCODES_H

#define NRF24_OK 					0
#define NRF24_INVALID_ARGUMENT 		-100 /* An argument is invalid */
#define NRF24_INVALID_HANDLE 		-101 /* A handle is invalid */
#define NRF24_OPEN_FAILED			-102 /* Failed to open resource */
#define NRF24_SETUP_FAILED			-103 /* Failed to configure resource */
#define NRF24_TRANSFER_FAILED		-104 /* Failed to receive or send */
#define NRF24_NOT_LISTENING			-105 /* Resource not configured for read */
#define NRF24_NO_DATA				-106 /* No data is available */
#define NRF24_MAX_RETRIES			-107 /* Reached maximum retries */
#define NRF24_TIMEOUT				-108 /* Operation timed out */

#endif /* NRF24_ERRORCCODES_H */
