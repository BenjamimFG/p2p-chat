#ifndef CONNECT_WINDOW_H
#define CONNECT_WINDOW_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

void draw_connect_window(WINDOW* window);
char* get_connect_ip_and_port(WINDOW* window);

#endif