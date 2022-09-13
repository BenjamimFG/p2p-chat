#include <string.h>
#include <stdlib.h>

#include "string_utils.h"

char* char_repeat(char c, int n) {
  char* str = malloc(n * sizeof(char));
  memset(str, 0, n);

  for (int i = 0; i < n; ++i) {
    str[i] = c;
  }

  return str;
}