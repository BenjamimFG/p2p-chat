#ifndef P2P_CHAT_H
#define P2P_CHAT_H

#include <netinet/in.h>
#include <sys/socket.h>

#define MAX_USERNAME_SIZE 20
#define MAX_MESSAGE_SIZE 80

typedef enum _server_state {
  WAITING_CONNECTIONS = 0,
  CONNECTION_ATTEMPT,
  ACCEPTED_REQUEST
} ServerState;

typedef struct _client {
  struct sockaddr_in client_addr;
  socklen_t client_addr_len;
  int client_fd;
  char username[MAX_USERNAME_SIZE + 1];
  char ip_address[16];
} Client;

typedef struct _message {
  char content[MAX_MESSAGE_SIZE + 1];
  char time_str[9];
  char username[MAX_USERNAME_SIZE + 1];
  int username_color;
} Message;

typedef struct _server_thread_args {
  int server_fd;
  volatile ServerState* server_state;
} ServerThreadArgs;

/**
 * Creates a TCP socket and set it to listen to port, returning
 * the socket file descriptor.
 * 
 * @param port Number of the port to listen to
 * 
 * @returns File descriptor of the server socket
*/
extern int start_server_and_listen(const int port);

/**
 * Thread function of the server. This function will continually wait for a client
 * and when a connection arrives will prompt the user to accept or deny it, if the
 * user accepts the connection the server_state is set as ACCEPTED_REQUEST and the 
 * function ends. This function changes the server_state passed in the argumnets
 * between WAITING_CONNECTIONS, CONNECTION_ATTEMPT and ACCEPTED_REQUEST accordingly.
 * 
 * @param args Arguments of the function, should actually be of type ServerThreadArgs*
 * 
 * @returns NULL
*/
extern void* server_thread_function(void* args);

/**
 * Connects to the host listening on address ipv4 and port over a TCP socket,
 * then sends the user's username so it can be read by the server side code.
 * 
 * @param ipv4 Pointer to the string of the ipv4 address to connect to
 * @param port Port the other user is running their p2p_chat on
 * @param username User's username to be sent on a connection attempt
 * 
 * @returns File descriptor of the client socket created for the connection, -1 if
 * cannot connect.
*/
extern int connect_to_server(const char* ipv4, const int port, const char* username);

#endif