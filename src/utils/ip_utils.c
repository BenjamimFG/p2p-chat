#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

#include "ip_utils.h"

Ipv4Port* parse_ipv4_and_port(const char* str) {
  Ipv4Port* result = malloc(sizeof(Ipv4Port));
  memset(result, 0, sizeof(Ipv4Port));

  // Copy string to avoid modifying original one
  int str_len = strlen(str);
  char* str_copy = malloc((str_len + 1) * sizeof(char));
  memset(str_copy, 0, (str_len + 1));
  strncpy(str_copy, str, str_len);

  // Split string at ':' and put first token into result->ip
  char* token = strtok(str_copy, ":");
  if (token != NULL)
    strncpy(result->ip, token, strlen(token));
  
  // Then put second token into result->port
  token = strtok(NULL, ":");
  if (token != NULL)
    strncpy(result->port, token, strlen(token));

  free(str_copy);

  if (!is_ipv4_valid(result->ip) || !is_port_valid(result->port))
    return NULL;

  return result;
}

bool is_ipv4_valid(const char* str) {
  if (str == NULL) return false;
  if (!isdigit(str[0])) return false;

  int str_len = strlen(str);

  // Check if there are consecutive dots '.'
  // e.g.: 192..168..1..1 is invalid
  for (int i = 1; i < str_len; ++i) {
    if (str[i] == '.' && str[i - 1] == '.')
      return false;
  }

  // Creates a copy of the string to avoid strtok modifying the original string
  char* ipv4 = malloc((str_len + 1) * sizeof(char));
  memset(ipv4, 0, (str_len + 1));
  strncpy(ipv4, str, str_len);

  // Can't directly return inside the while loop cause we need to
  // free ipv4, so either free(ipv4) multiple times inside the ifs
  // in the while loop or just save the is_valid and free at the end
  bool is_valid = true;

  // total_numbers will be the total of 'number components' of the ip
  // e.g.: 12.12.12 would have total_numbers = 3 (3 number components)
  // so if total_numbers is bigger or smaller than 4 the ipv4 is already invalid
  int total_numbers = 0;
  char* number_token = strtok(ipv4, ".");

  while(number_token != NULL && is_valid) {
    if ((++total_numbers) > 4) is_valid = false;

    bool number_is_zero = strcmp(number_token, "0") == 0;
    
    // If number is zero it's already valid so just continue
    // e.g.: 0.0.0.0 is valid
    if (!number_is_zero) {
      if (number_token[0] == '0') is_valid = false;

      int int_number = atoi(number_token);

      if (int_number <= 0 || int_number > 256) is_valid = false;
    }

    number_token = strtok(NULL, ".");
  }

  free(ipv4);

  if (total_numbers < 4) return false;

  return is_valid;
}

bool is_port_valid(const char* port) {
  if (port == NULL) return false;
  if (port[0] == '0') return false;
  
  // Check if all characters in port are digits
  int len = strlen(port);
  for (int i = 0; i < len; ++i) {
    if (!isdigit(port[i]))
      return false;
  }

  int port_number = atoi(port);

  // Valid port numbers
  return port_number > 0 && port_number <= 65535;
}
