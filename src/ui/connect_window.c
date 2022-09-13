#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#include "connect_window.h"

void draw_connect_window(WINDOW* window) {
  box(window, 0, 0);

  mvwprintw(window, 1, 1, "Type the ip and port to connect to: (format: 256.256.256.256:65535)");

  mvwprintw(window, 3, 1, "> ");

  wrefresh(window);
}

char* get_connect_ip_and_port(WINDOW* window) {
  echo();
  curs_set(1);

  // 22 because the longest ip port of ipv4 is 21
  // characters long (the extra character is for \0)
  char* ip_and_port = malloc(22 * sizeof(char));
  memset(ip_and_port, 0, 22);

  wgetnstr(window, ip_and_port, 21);

  return ip_and_port;
}

