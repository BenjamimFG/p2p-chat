#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * Takes a character 'c' and repeats it 'n' times into a string.
 * 
 * @param c Character to be repeated
 * @param n Number of times to repeat char c
 * 
 * @return Returns a pointer to a string of length 'n'
 */
char* char_repeat(const char c, const int n);

/**
 * Checks if a string starts with a character.
 * 
 * @param str Pointer to the string to be checked
 * @param c Character to check if it's the first on str
 * 
 * @returns Returns true if str starts with c, false otherwise
*/
bool starts_with(const char* str, const char c);

#endif