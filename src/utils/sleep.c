// Macro to enable POSIX extension, this enables usage of nanosleep function from time.h
#define _POSIX_C_SOURCE 200809L

#include <time.h>

#include "sleep.h"

// constant equivalent of 1 millisecond in nanoseconds
#define ONE_MS_IN_NSEC 1000000

void ms_sleep(const int ms) {
  struct timespec ts = {0};

  ts.tv_nsec = (ms % 1000) * ONE_MS_IN_NSEC;

  if (ms >= 1000)
    ts.tv_sec = ms/1000;

  nanosleep(&ts, NULL);
}