#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string.h>
#include <stdlib.h>

/**
 * Takes a character 'c' and repeats it 'n' times into a string.
 * 
 * @param c Character to be repeated
 * @param n Number of times to repeat char c
 * 
 * @return Returns a pointer to a string of length 'n'
 */
char* char_repeat(char c, int n);

#endif