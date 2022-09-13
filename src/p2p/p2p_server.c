#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
#include <stdbool.h>

#include "p2p_server.h"
#include "yes_no_window.h"
#include "window_utils.h"

/**
 * Creates a TCP socket and set it to listen to port, returning the socket file descriptor
 * @param port Number of the port to listen to
 * @returns File descriptor of the server socket
*/
int start_server_and_listen(int port) {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (server_fd < 0) {
    perror("Error while creating listener socket");
    exit(EXIT_FAILURE);
  }

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

  if (listen(server_fd, 3) < 0) {
    perror("Error when trying to listen to socket");
    exit(EXIT_FAILURE);
  }

  return server_fd;
}

Client* wait_for_client(int server_fd) {
  Client* client = (Client*) malloc(sizeof(Client));
  memset(client, 0, sizeof(Client));

  client->client_fd = accept(server_fd, (struct sockaddr*) &client->client_addr, &client->client_addr_len);
  
  if (client->client_fd < 0) {
    perror("Error when trying to create new accept socket");
    exit(EXIT_FAILURE);
  }

  // Convert client address into readable format and save into client->ip_address
  inet_ntop(AF_INET, &client->client_addr.sin_addr.s_addr, client->ip_address, sizeof client->ip_address);

  // Read client username
  read(client->client_fd, client->username, sizeof client->username);

  return client;
}

void* server_thread_function(void* args) {
  ServerThreadArgs* typed_args = (ServerThreadArgs*) args;

  while(1) {
    *typed_args->server_state = WAITING_CONNECTIONS;

    Client* client = wait_for_client(typed_args->server_fd);

    *typed_args->server_state = CONNECTION_ATTEMPT;

    WINDOW* accept_window = create_window_centered(16, 82);

    char question[80] = {0};
    sprintf(question, "Accept request to chat from %s (%s)?", client->username, client->ip_address);

    draw_yes_no_window(accept_window, question);

    bool accept = get_answer(accept_window);

    delwin(accept_window);

    if (accept) {
      *typed_args->server_state = ACCEPTED_REQUEST;

      break;
    }
    
    //char* buf = "Hello World!";

    //write(client_fd, buf, strlen(buf));

    close(client->client_fd);
  }

  return NULL;
}