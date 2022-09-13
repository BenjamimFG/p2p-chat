#include <time.h>

#include "time_utils.h"

void get_time_str(char* output) {
  time_t rawtime = time(NULL);
  struct tm* timeinfo = localtime(&rawtime);

  strftime(output, 9, "%T", timeinfo);
}