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
 * @param peer_output Pointer to Peer struct that will be written with the output of connected peer 
*/
void wait_for_peer(const int server_fd, Peer* peer_output) {
  // Blocks here until a connection is received
  peer_output->fd = accept(server_fd, (struct sockaddr*) &peer_output->address, &peer_output->address_len);
  
  if (peer_output->fd < 0) {
    perror("Error when trying to create peer socket");
    exit(EXIT_FAILURE);
  }

  // Convert peer address into readable format and save into peer->ipv4
  inet_ntop(AF_INET, &peer_output->address.sin_addr.s_addr, peer_output->ipv4, sizeof peer_output->ipv4);

  // Read peer username
  recv(peer_output->fd, peer_output->username, MAX_USERNAME_SIZE, 0);
}

extern void* server_thread_function(void* args) {
  ServerThreadArgs* typed_args = (ServerThreadArgs*) args;

  while(true) {
    *typed_args->server_state = WAITING_CONNECTIONS;

    // Blocks until a connection attempt is received
    wait_for_peer(typed_args->server_fd, typed_args->connected_peer);

    *typed_args->server_state = CONNECTION_ATTEMPT;

    // Prompt user to accept the request
    WINDOW* accept_window = create_window_centered(16, 82, true);

    char question[80] = {0};
    sprintf(question, "Accept request to chat from %s (%s)? [y/N]", typed_args->connected_peer->username, typed_args->connected_peer->ipv4);

    bool accept = get_y_n(accept_window, question, 1, 1);

    delwin(accept_window);

    if (accept) {
      // Send username to peer
      send(typed_args->connected_peer->fd, typed_args->username, MAX_USERNAME_SIZE, 0);

      *typed_args->server_state = ACCEPTED_REQUEST;

      break;
    }

    // Sends an empty message, the peer trying to connect should understand
    // that an empty message means the connection was refused
    send(typed_args->connected_peer->fd, NULL, 0, 0);

    close(typed_args->connected_peer->fd);

    // Reset connected_peer struct to 0
    memset(typed_args->connected_peer, 0, sizeof(Peer));
  }

  return NULL;
}

extern Peer* connect_to_peer(const char* ipv4, const int port, const char* username) {
  Peer* peer = (Peer*) malloc(sizeof(Peer));
  memset(peer, 0, sizeof(Peer));

  peer->address.sin_family = AF_INET;
  peer->address.sin_port = htons(port);

  // Should not fail here if ipv4 is valid
  if (inet_pton(AF_INET, ipv4, &peer->address.sin_addr) <= 0) {
    perror("Error on connect to peer, invalid ipv4 address.");
    exit(EXIT_FAILURE);
  }

  peer->address_len = sizeof(peer->address);

  peer->fd = socket(AF_INET, SOCK_STREAM, 0);
  if (peer->fd < 0) {
    perror("Error when trying to create peer socket.");
    exit(EXIT_FAILURE);
  }

  if (connect(peer->fd, (struct sockaddr*) &peer->address, sizeof(peer->address)) < 0)
    return NULL;

  // Send username to peer
  send(peer->fd, username, strlen(username), 0);

  // Receive peer's username
  recv(peer->fd, peer->username, MAX_USERNAME_SIZE, 0);

  // If the message received was empty (0 characters) the connection was refused
  if (strlen(username) == 0)
    close(peer->fd);

  return peer;
}