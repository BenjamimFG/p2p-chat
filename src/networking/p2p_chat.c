#include <arpa/inet.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "p2p_chat.h"
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
 * socket at server_fd and reads first message as peer's username.
 * 
 * @param server_fd File descriptor of the server which is listening for peer connections
 * 
 * @returns Pointer to Peer struct with the information of the peer that tried to connect
*/
Peer* wait_for_peer(const int server_fd) {
  Peer* peer = (Peer*) malloc(sizeof(Peer));
  memset(peer, 0, sizeof(Peer));

  // Blocks here until a connection is received
  peer->fd = accept(server_fd, (struct sockaddr*) &peer->address, &peer->address_len);
  
  if (peer->fd < 0) {
    perror("Error when trying to create peer socket");
    exit(EXIT_FAILURE);
  }

  // Convert peer address into readable format and save into peer->ipv4
  inet_ntop(AF_INET, &peer->address.sin_addr.s_addr, peer->ipv4, sizeof peer->ipv4);

  // Read peer username
  read(peer->fd, peer->username, sizeof peer->username);

  return peer;
}

extern void* server_thread_function(void* args) {
  ServerThreadArgs* typed_args = (ServerThreadArgs*) args;

  while(true) {
    *typed_args->server_state = WAITING_CONNECTIONS;

    // Blocks until a connection attempt is received
    Peer* peer = wait_for_peer(typed_args->server_fd);

    *typed_args->server_state = CONNECTION_ATTEMPT;

    // Prompt user to accept the request
    WINDOW* accept_window = create_window_centered(16, 82, true);

    char question[80] = {0};
    sprintf(question, "Accept request to chat from %s (%s)? [y/N]", peer->username, peer->ipv4);

    bool accept = get_y_n(accept_window, question, 1, 1);

    delwin(accept_window);

    if (accept) {
      // do stuff in the future
      *typed_args->server_state = ACCEPTED_REQUEST;

      break;
    }

    close(peer->fd);
  }

  return NULL;
}

extern int connect_to_peer(const char* ipv4, const int port, const char* username) {
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
    perror("Error when trying to create peer socket.");
    exit(EXIT_FAILURE);
  }

  if (connect(client_fd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
    return -1;

  send(client_fd, username, strlen(username), 0);

  return client_fd;
}