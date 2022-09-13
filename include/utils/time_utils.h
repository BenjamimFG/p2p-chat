#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <time.h>

/**
 * Gets the current time and stores it formatted to the output pointer.
 * Uses the format hh:mm:ss (a.k.a. "%T" from strftime).
 * 
 * @param output String pointer where output will be stored. (Needs to be at least 9 characters of memory)
*/
void get_time_str(char* output);

#endif