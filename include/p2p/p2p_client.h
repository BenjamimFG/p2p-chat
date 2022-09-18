#ifndef P2P_CLIENT_H
#define P2P_CLIENT_H

#include <stdbool.h>

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
int connect_to_server(const char* ipv4, const int port, const char* username);

#endif