#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <locale.h>

#include "p2p_server.h"
#include "main_menu.h"
#include "window_utils.h"
#include "connect_window.h"
#include "ip_utils.h"
#include "error_window.h"

/**
 * Prints program usage to the stderr stream
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

  /// SERVER
  int server_fd = start_server_and_listen(port);
  volatile ServerState server_state = WAITING_CONNECTIONS;

  ServerThreadArgs server_args = {
    .server_fd = server_fd,
    .server_state = &server_state
  };

  // The server thread will keep listening and waiting for a client to connect
  // in the background, until the user accepts a connection or selects an option
  // in the main menu
  pthread_t server_thread;
  pthread_create(&server_thread, NULL, &server_thread_function, (void*) &server_args);
  ///

  setlocale(LC_ALL, "");
  initscr();

  WINDOW* main_menu_window = create_window_centered(MAIN_MENU_HEIGHT, MAIN_MENU_WIDTH);

  // Volatile cause this will be changed in another thread
  volatile MainMenuOption menu_res = NO_OPTION;

  MenuThreadArgs menu_args = {
    .window = main_menu_window,
    .result = &menu_res
  };

  strcpy(menu_args.username, username);

  // Main menu thread will run parallel to the server thread, whichever ends 
  pthread_t menu_thread;
  pthread_create(&menu_thread, NULL, &menu_thread_func, (void*) &menu_args);

  // Queue* message_queue = queue_create();

  // This keeps looping until the user selects an option in 
  // the menu or the user receives a connection request
  while (TRUE) {
    if (menu_res == NO_OPTION && server_state == WAITING_CONNECTIONS) continue;

    if (server_state == CONNECTION_ATTEMPT) {
      pthread_cancel(menu_thread);

      // Wait while user decides to accept or refuse connection attempt
      while (server_state == CONNECTION_ATTEMPT) {}

      // If request was accepted
      if (server_state == ACCEPTED_REQUEST) {
        printf("Accepted Request\n");
        break;
      }

      // If it wasnt then the only other possible server_state is WAITING_CONNECTIONS again
      // then the menu thread is restarted and the screen is cleared
      clear();
      refresh();

      menu_res = NO_OPTION;

      pthread_create(&menu_thread, NULL, &menu_thread_func, (void*) &menu_args);

      continue;
    }

    switch(menu_res) {
      case CONNECT_PEER:
        pthread_cancel(server_thread);
        delwin(main_menu_window);
        clear();
        refresh();

        WINDOW* connect_window = create_window_centered(12, 82);

        draw_connect_window(connect_window);
        add_title(connect_window, "Connect to peer");

        char* ip_and_port = get_connect_ip_and_port(connect_window);

        Ipv4Port* parsed_connect_address = parse_ipv4_and_port(ip_and_port);

        delwin(connect_window);

        clear();
        refresh();

        if (parsed_connect_address == NULL) {
          WINDOW* error_window = create_window_centered(14, 60);
          char error_msg[30] = "Invalid connect ip or port.";
          draw_error_window(error_window, error_msg);

          delwin(error_window);
          
          endwin();
          return EXIT_FAILURE;
        }
        
        break;
      // Should never be NO_OPTION here, since NO_OPTION is handled
      // in the if above, but added to avoid compiler warnings
      case NO_OPTION:
      case EXIT:
        pthread_cancel(server_thread);
        delwin(main_menu_window);
        break;
    }

    break;
  }

  pthread_join(menu_thread, NULL);
  pthread_join(server_thread, NULL);

  close(server_fd);

  endwin();

  return EXIT_SUCCESS;
}
