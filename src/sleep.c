#include "sleep.h"
#include <stddef.h>
#include <time.h>
#include <sys/time.h>

void nrf24_msleep(const uint32_t milliseconds)
{
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = milliseconds * 1000000L;
  nanosleep(&req, (struct timespec *)NULL);
}

void nrf24_usleep(const uint32_t microseconds)
{
  struct timespec req;
  req.tv_sec = 0;
  req.tv_nsec = microseconds * 1000L;
  nanosleep(&req, (struct timespec *)NULL);  
}
