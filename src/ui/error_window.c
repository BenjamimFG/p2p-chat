#include <ncurses.h>
#include <string.h>

#include "error_window.h"
#include "window_utils.h"

void draw_error_window(WINDOW* window, const char* msg) {
  add_title(window, "Error");

  int start_y = (getmaxy(window) - 2) / 2;
  int start_x = (getmaxx(window) - strlen(msg) - 2) / 2;

  mvwprintw(window, start_y, start_x, msg);

  mvwprintw(window, getmaxy(window) - 2, 1, "Press any key to continue...");

  wrefresh(window);

  noecho();
  curs_set(0);

  getch();
}