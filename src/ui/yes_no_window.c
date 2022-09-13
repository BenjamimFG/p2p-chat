#include <ncurses.h>
#include <stdbool.h>

void draw_yes_no_window(WINDOW* window, char* question) {
  box(window, 0, 0);

  mvwprintw(window, 1, 1, "%s [y/N]", question);

  wrefresh(window);
}

int get_answer(WINDOW* window) {
  noecho();
  curs_set(0);

  char c = wgetch(window);

  switch (c) {
  case 'y':
  case 'Y':
    return true;
    break;
  
  default:
    break;
  }

  return false;
}
