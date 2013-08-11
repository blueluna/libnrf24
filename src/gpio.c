/* 
 * Copyright (c) 2013, Erik Svensson <erik.public@gmail.com>
 */

#include "gpio.h"
#include "errorcodes.h"
#include <stdio.h>

int32_t nrf24_gpio_open(const uint16_t port, const uint8_t direction)
{
  FILE *f;
  char file[128];
  f = fopen("/sys/class/gpio/export", "w");
  if (f == 0) {
    return NRF24_OPEN_FAILED;
  }
  fprintf(f, "%u\n", port);
  fclose(f);
  
  sprintf(file, "/sys/class/gpio/gpio%u/direction", port);
  f = fopen(file, "w");
  if (f == 0) {
    return NRF24_OPEN_FAILED;
  }
  if (direction == NRF24_GPIO_INPUT) {
    fprintf(f, "in\n");
  }
  else {
    fprintf(f, "out\n");
  }
  fclose(f);
  return NRF24_OK;
}

int32_t nrf24_gpio_close(const uint16_t port)
{
  FILE *f;
  f = fopen("/sys/class/gpio/unexport", "w");
  if (f == 0) {
    return NRF24_OPEN_FAILED;
  }
  fprintf(f, "%d\n", port);
  fclose(f);
  return NRF24_OK;
}

int32_t nrf24_gpio_read(const uint16_t port, uint8_t *value)
{
  int i;
  FILE *f;
  char file[128];
  sprintf(file, "/sys/class/gpio/gpio%u/value", port);
  f = fopen(file, "r");
  if (f == 0) {
    return NRF24_OPEN_FAILED;
  }
  fscanf(f, "%d", &i);
  if (value == 0) {
    *value = i;
  }
  fclose(f);
  return i;
}

int32_t nrf24_gpio_write(const uint16_t port, const uint8_t value)
{
  FILE *f;
  char file[128];
  sprintf(file, "/sys/class/gpio/gpio%u/value", port);
  f = fopen(file, "w");
  if (f == 0) {
    return NRF24_OPEN_FAILED;
  }
  if (value == NRF24_GPIO_LOW) {
    fprintf(f, "0\n");
  }
  else {
    fprintf(f, "1\n");
  }
  fclose(f);
  return NRF24_OK;
}
