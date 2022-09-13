#ifndef P2P_SERVER_H
#define P2P_SERVER_H

#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_USERNAME_SIZE 20
#define MAX_MESSAGE_SIZE 80

typedef enum _server_state {
  WAITING_CONNECTIONS = 0,
  CONNECTION_ATTEMPT,
  ACCEPTED_REQUEST
} ServerState;

typedef struct Client {
  struct sockaddr_in client_addr;
  socklen_t client_addr_len;
  int client_fd;
  char username[MAX_USERNAME_SIZE + 1];
  char ip_address[16];
} Client;

typedef struct Message {
  char content[MAX_MESSAGE_SIZE + 1];
  char time_str[9];
  char username[MAX_USERNAME_SIZE + 1];
  int username_color;
} Message;

typedef struct _server_thread_args {
  int server_fd;
  volatile ServerState* server_state;
} ServerThreadArgs;

int start_server_and_listen(int port);
Client* wait_for_client(int server_fd);
void* server_thread_function(void* args);

#endif