#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"

char* char_repeat(const char c, const int n) {
  char* str = malloc(n * sizeof(char));
  memset(str, 0, n * sizeof(char));

  for (int i = 0; i < n; ++i) {
    str[i] = c;
  }

  return str;
}

bool starts_with(const char* str, const char c) {
  if (strlen(str) == 0) return false;

  return str[0] == c;
}