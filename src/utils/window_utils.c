#include <ncurses.h>

#include "window_utils.h"

void add_title(WINDOW* window, const char* title) {
  // Save initial cursor position
  int old_y, old_x;
  getyx(window, old_y, old_x);

  mvwprintw(window, 0, 2, " %s ", title);

  // Move back to initial position
  wmove(window, old_y, old_x);

  wrefresh(window);
}

WINDOW* create_window_centered(const int height, const int width) {
  int screen_height, screen_width;
  getmaxyx(stdscr, screen_height, screen_width);

  int start_y = (screen_height - height)/2;
  int start_x = (screen_width - width)/2;

  // Check if start_y and start_x are valid (not negative)
  start_y = start_y > 0 ? start_y : 0;
  start_x = start_x > 0 ? start_x : 0;

  WINDOW* window = newwin(height, width, start_y, start_x);
  
  // Refresh stdscr so it includes our newly created window
  refresh();

  return window;
}