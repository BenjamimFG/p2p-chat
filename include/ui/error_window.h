#ifndef ERROR_WINDOW_H
#define ERROR_WINDOW_H

#include <ncurses.h>
#include <string.h>

#include "window_utils.h"

extern void draw_error_window(WINDOW* window, const char* msg);

#endif