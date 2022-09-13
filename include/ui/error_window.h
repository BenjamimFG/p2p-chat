#ifndef ERROR_WINDOW_H
#define ERROR_WINDOW_H

#include <ncurses.h>
#include <string.h>

#include "window_utils.h"

void draw_error_window(WINDOW* window, char* msg);

#endif