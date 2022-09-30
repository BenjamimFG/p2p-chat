#include <locale.h>
#include <ncurses.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "error_window.h"
#include "ip_utils.h"
#include "main_menu.h"
#include "p2p_chat.h"
#include "window_utils.h"

/**
 * Prints program usage to the stderr stream
 * 
 * @param bin_name Name of the binary to be displayed in the usage message (usually argv[0])
*/
void usage(const char* bin_name) {
  fprintf(stderr, "Usage:\n");
  fprintf(stderr, "\t%s PORT USERNAME\n", bin_name);
}

int main(int argc, char const *argv[]) {
  if (argc < 3) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  int port = atoi(argv[1]);
  if (port == 0) {
    fprintf(stderr, "Invalid port: \"%s\"\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  char username[MAX_USERNAME_SIZE + 1] = {0};
  strncpy(username, argv[2], MAX_USERNAME_SIZE);

  // Peer struct of the other chat user connected to
  Peer* connected_peer = (Peer*) malloc(sizeof(Peer));
  memset(connected_peer, 0, sizeof(Peer));

  /// SERVER ///
  int server_fd = start_server_and_listen(port);
  // Volatile cause this will be changed in another thread
  volatile ServerState server_state = WAITING_CONNECTIONS;

  ServerThreadArgs server_args = {
    .server_fd = server_fd,
    .server_state = &server_state,
    .connected_peer = connected_peer
  };

  strncpy(server_args.username, username, MAX_USERNAME_SIZE);

  // The server thread will keep listening and waiting for a peer to connect
  // in the background, until the user accepts a connection or selects an option
  // in the main menu
  pthread_t server_thread;
  pthread_create(&server_thread, NULL, &server_thread_function, (void*) &server_args);
  /// END SERVER ///

  setlocale(LC_ALL, "");
  initscr();

  /// MAIN MENU ///
  // Volatile cause this will be changed in another thread
  volatile MainMenuOption menu_res = NO_OPTION;

  MenuThreadArgs menu_args = {
    .result = &menu_res
  };

  strcpy(menu_args.username, username);

  // Main menu thread will run parallel to the server thread, whichever ends 
  pthread_t menu_thread;
  pthread_create(&menu_thread, NULL, &menu_thread_func, (void*) &menu_args);
  /// END MAIN MENU ///

  // This keeps looping until the user selects an option in 
  // the menu or the user receives a connection request
  while (true) {
    if (menu_res == NO_OPTION && server_state == WAITING_CONNECTIONS) continue;

    if (server_state == CONNECTION_ATTEMPT) {
      pthread_cancel(menu_thread);

      // Wait while user decides to accept or refuse connection attempt
      while (server_state == CONNECTION_ATTEMPT) {}

      // If request was accepted
      if (server_state == ACCEPTED_REQUEST) {
        break;
      }

      // If it wasnt then the only other possible server_state is WAITING_CONNECTIONS again
      // then the menu thread is restarted and the screen is cleared
      clear_screen();
      pthread_create(&menu_thread, NULL, &menu_thread_func, (void*) &menu_args);

      continue;
    }

    switch(menu_res) {
      case CONNECT_PEER:
        pthread_cancel(server_thread);
        clear_screen();

        WINDOW* connect_window = create_window_centered(12, 82, true);

        add_title(connect_window, "Connect to peer");
        add_header(connect_window, "Type the ip and port to connect to: (format: 256.256.256.256:65535)");

        char* ip_and_port = get_string(connect_window, 22, "> ", 3, 1);

        Ipv4Port* parsed_connect_address = parse_ipv4_and_port(ip_and_port);

        delwin(connect_window);

        clear_screen();

        if (parsed_connect_address == NULL) {
          WINDOW* error_window = create_window_centered(14, 60, true);

          draw_error_window(error_window, "Invalid connect ip or port.");

          // Restart menu and server thread
          clear_screen();
          pthread_create(&menu_thread, NULL, &menu_thread_func, (void*) &menu_args);
          pthread_create(&server_thread, NULL, &server_thread_function, (void*) &server_args);

          continue;
        }
        
        // Should be valid cause parsed_connect_address was validated before
        int server_port = atoi(parsed_connect_address->port);

        connected_peer = connect_to_peer(parsed_connect_address->ip, server_port, username);

        if (connected_peer == NULL || strlen(connected_peer->username) == 0) {
          WINDOW* error_window = create_window_centered(14, 60, true);

          char err_msg[70];

          if (connected_peer == NULL)
            snprintf(err_msg, 70, "Could not connect to peer at %s:%d", parsed_connect_address->ip, server_port);
          else if (strlen(connected_peer->username) == 0)
            snprintf(err_msg, 70, "Peer %s:%d refused connection", parsed_connect_address->ip, server_port);

          draw_error_window(error_window, err_msg);

          // Reset connected_peer struct to 0
          memset(connected_peer, 0, sizeof(Peer));

          // Restart menu and server thread
          clear_screen();
          pthread_create(&menu_thread, NULL, &menu_thread_func, (void*) &menu_args);
          pthread_create(&server_thread, NULL, &server_thread_function, (void*) &server_args);

          continue;
        }
        
        break;
      // Should never be NO_OPTION here, since NO_OPTION is handled
      // in the if above, but added to avoid compiler warnings
      case NO_OPTION:
      case EXIT:
        pthread_cancel(server_thread);
        break;
    }

    break;
  }

  pthread_join(menu_thread, NULL);
  pthread_join(server_thread, NULL);

  // If address_len is > 0 then a connection was made either through the
  // server_thread or the user connected through the main menu option
  if (connected_peer->address_len > 0) {
    // TODO: Start chat here

    close(connected_peer->fd);
  }

  close(server_fd);

  endwin();

  return EXIT_SUCCESS;
}
