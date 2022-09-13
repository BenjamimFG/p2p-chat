#ifndef YES_NO_WINDOW_H
#define YES_NO_WINDOW_H

#include <ncurses.h>
#include <stdbool.h>

void draw_yes_no_window(WINDOW* window, char* question);
int get_answer(WINDOW* window);

#endif