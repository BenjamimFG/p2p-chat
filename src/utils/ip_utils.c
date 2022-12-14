#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ip_utils.h"

/**
 * Check if a ipv4 string is valid.
 * 
 * @param str Pointer to the string to be checked.
 * 
 * @returns true if str is a valid ipv4, false otherwise.
*/
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

  // total_numbers will be the total of 'number components' of the ip
  // e.g.: 12.12.12 would have total_numbers = 3 (3 number components)
  // so if total_numbers is bigger or smaller than 4 the ipv4 is already invalid
  int total_numbers = 0;
  char* number_token = strtok(ipv4, ".");

  // The string starts as valid if it got till here
  bool is_valid = true;

  while(number_token != NULL) {
    // Every time we get inside the loop set is_valid to false and only
    // set it back to true if the loop gets to the end successfully
    is_valid = false;

    if ((++total_numbers) > 4) break;

    bool number_is_zero = strcmp(number_token, "0") == 0;
    
    // If number is zero it's already valid so just continue
    // e.g.: 0.0.0.0 is valid
    if (!number_is_zero) {
      // Can't start with 0 e.g.: 026.012.01.0256 is invalid
      if (number_token[0] == '0') break;

      int int_number = atoi(number_token);

      if (int_number <= 0 || int_number > 256) break;
    }

    number_token = strtok(NULL, ".");
    // Set is_valid back to true if the loop finished succesfully
    is_valid = true;
  }

  free(ipv4);

  return is_valid;
}

/**
 * Check if a port string is valid.
 * 
 * @param port Pointer to the string to be checked.
 * 
 * @returns true if port is a valid port number, false otherwise.
*/
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