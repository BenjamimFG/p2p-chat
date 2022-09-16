#include <arpa/inet.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "p2p_server.h"
#include "window_utils.h"

extern int start_server_and_listen(const int port) {
  // Create ipv4 socket
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("Error while creating server socket");
    exit(EXIT_FAILURE);
  }

  // Struct that holds the address we'll bind to, family is ipv4
  // INADDR_ANY makes it so the socket can receive packets from all interfaces
  // htons needs to be called on the port to convert from host to network byte order
  const struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = htonl(INADDR_ANY),
    .sin_port = htons(port)
  };

  if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
    char err[100];
    sprintf(err, "Error when binding to port %d", port);
    perror(err);
    exit(EXIT_FAILURE);
  }

  // Queue up to 3 connections, shouldn't need many more in a simple p2p application
  if (listen(server_fd, 3) < 0) {
    perror("Error when trying to listen to socket");
    exit(EXIT_FAILURE);
  }

  return server_fd;
}

/**
 * Blocking function, wait until someone tries to connect to the
 * socket at server_fd and reads first message as client's username.
 * 
 * @param server_fd File descriptor of the server which is listening for client connections
 * 
 * @returns Pointer to Client struct with the information of the client that tried to connect
*/
Client* wait_for_client(const int server_fd) {
  Client* client = (Client*) malloc(sizeof(Client));
  memset(client, 0, sizeof(Client));

  // Blocks here until a connection is received
  client->client_fd = accept(server_fd, (struct sockaddr*) &client->client_addr, &client->client_addr_len);
  
  if (client->client_fd < 0) {
    perror("Error when trying to create client socket");
    exit(EXIT_FAILURE);
  }

  // Convert client address into readable format and save into client->ip_address
  inet_ntop(AF_INET, &client->client_addr.sin_addr.s_addr, client->ip_address, sizeof client->ip_address);

  // Read client username
  read(client->client_fd, client->username, sizeof client->username);

  return client;
}

extern void* server_thread_function(void* args) {
  ServerThreadArgs* typed_args = (ServerThreadArgs*) args;

  while(true) {
    *typed_args->server_state = WAITING_CONNECTIONS;

    // Blocks until a connection attempt is received
    Client* client = wait_for_client(typed_args->server_fd);

    *typed_args->server_state = CONNECTION_ATTEMPT;

    // Prompt user to accept the request
    WINDOW* accept_window = create_window_centered(16, 82, true);

    char question[80] = {0};
    sprintf(question, "Accept request to chat from %s (%s)? [y/N]", client->username, client->ip_address);

    bool accept = get_y_n(accept_window, question, 1, 1);

    delwin(accept_window);

    if (accept) {
      // do stuff in the future
      *typed_args->server_state = ACCEPTED_REQUEST;

      break;
    }

    close(client->client_fd);
  }

  return NULL;
}