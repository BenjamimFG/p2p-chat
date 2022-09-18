#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

#include "p2p_client.h"

int connect_to_server(const char* ipv4, const int port, const char* username) {
  struct sockaddr_in server_address = {
    .sin_family = AF_INET,
    .sin_port = htons(port)
  };

  // Should not fail here if ipv4 is valid
  if (inet_pton(AF_INET, ipv4, &server_address.sin_addr) <= 0) {
    perror("Error on connect to server, invalid ipv4 address.");
    exit(EXIT_FAILURE);
  }

  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0) {
    perror("Error when trying to create client socket.");
    exit(EXIT_FAILURE);
  }

  if (connect(client_fd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
    return -1;

  send(client_fd, username, strlen(username), 0);

  return client_fd;
}