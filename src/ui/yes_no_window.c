#include <ncurses.h>
#include <stdbool.h>

void draw_yes_no_window(WINDOW* window, const char* question) {
  box(window, 0, 0);

  mvwprintw(window, 1, 1, "%s [y/N]", question);

  wrefresh(window);
}

bool get_answer(WINDOW* window) {
  noecho();
  curs_set(0);

  char c = wgetch(window);

  return c == 'y' || c == 'Y';
}
