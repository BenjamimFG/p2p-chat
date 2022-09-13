#ifndef IP_UTILS_H
#define IP_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

typedef struct _ipv4_port {
  char ip[16];
  char port[6];
} Ipv4Port;

/**
 * Takes a string in the format "256.256.256.256:65535"
 * and parses the ip and port into a Ipv4Port struct.
 * 
 * @param str Pointer to the string to be parsed
 * 
 * @returns Pointer to the Ipv4Port struct with the parsed
 * result or NULL if the string is invalid.
*/
Ipv4Port* parse_ipv4_and_port(const char* str);

/**
 * Check if a ipv4 string is valid.
 * 
 * @param str Pointer to the string to be checked.
 * 
 * @returns true if str is a valid ipv4, false otherwise.
*/
bool is_ipv4_valid(const char* str);

/**
 * Check if a port string is valid.
 * 
 * @param port Pointer to the string to be checked.
 * 
 * @returns true if port is a valid port number, false otherwise.
*/
bool is_port_valid(const char* port);

#endif