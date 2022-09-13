#include <ncurses.h>
#include <string.h>

#include "p2p_server.h"
#include "main_menu.h"
#include "window_utils.h"

char options[N_OPTIONS][20] = {
  "Connect to peer",
  "Exit"
};

int selected = CONNECT_PEER;

void draw_menu(WINDOW* window, char* username) {  
  box(window, 0, 0);

  add_title(window, "Main menu");

  mvwprintw(window, 1, 1, "Connected as: %s", username);

  for (int i = 0; i < N_OPTIONS; ++i) {
    if (selected == i) wattron(window, A_STANDOUT);
    else wattroff(window, A_STANDOUT);

    if (i == EXIT)
      mvwprintw(window, MAIN_MENU_HEIGHT - 2, 1, "0. %-20s", options[i]);
    else
      mvwprintw(window, 3+i, 1, "%d. %-20s", i + 1, options[i]);
  }
  wattroff(window, A_STANDOUT);

  wrefresh(window);
}

MainMenuOption select_item(WINDOW* window, char* username) {
  noecho();
  keypad(window, TRUE);
  curs_set(0);

  int key;

  while ((key = wgetch(window)) != '\n') {
    switch (key) {
      case KEY_DOWN:
        selected++;
        selected = selected % (N_OPTIONS);

        draw_menu(window, username);
        break;
      case KEY_UP:
        selected--;
        if (selected < 0) selected = (N_OPTIONS - 1);

        draw_menu(window, username);
        break;
      default:
        break;
    }
  }

  return selected;
}

void* menu_thread_func(void* args) {
  MenuThreadArgs typed_args = *(MenuThreadArgs*) args;

  WINDOW* window = typed_args.window;
  char username[MAX_USERNAME_SIZE];
  strcpy(username, typed_args.username);

  draw_menu(window, username);
  int selected = select_item(window, username);

  *typed_args.result = selected;

  return NULL;
}