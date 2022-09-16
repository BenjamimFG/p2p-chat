#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
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

WINDOW* create_window_centered(const int height, const int width, const bool is_boxed) {
  int screen_height, screen_width;
  getmaxyx(stdscr, screen_height, screen_width);

  int start_y = (screen_height - height)/2;
  int start_x = (screen_width - width)/2;

  // Check if start_y and start_x are valid (not negative)
  start_y = start_y > 0 ? start_y : 0;
  start_x = start_x > 0 ? start_x : 0;

  WINDOW* window = newwin(height, width, start_y, start_x);

  if (is_boxed)
    box(window, 0, 0);
  
  // Refresh stdscr so it includes our newly created window
  refresh();

  return window;
}

void add_header(WINDOW* window, const char* header) {
  mvwprintw(window, 1, 1, header);

  wrefresh(window);
}

char* get_string(WINDOW* window, const int n, const char* prompt,
                 const int start_y, const int start_x) {
  echo();
  curs_set(1);

  mvwprintw(window, start_y, start_x, prompt);
  wrefresh(window);

  char* str_output = malloc(n * sizeof(char));
  memset(str_output, 0, n);

  wgetnstr(window, str_output, n);

  return str_output;
}

bool get_y_n(WINDOW* window, const char* prompt, const int start_y, const int start_x) {
  noecho();
  curs_set(0);

  mvwprintw(window, start_y, start_x, prompt);
  wrefresh(window);

  const char c = wgetch(window);

  return c == 'y' || c == 'Y';
}

void clear_screen() {
  clear();
  refresh();
}