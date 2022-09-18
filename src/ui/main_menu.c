#include <ncurses.h>
#include <string.h>

#include "main_menu.h"
#include "p2p_server.h"
#include "window_utils.h"

// Number of options in the menu
#define N_OPTIONS 2
// Default option to be selected
#define DEFAULT_OPTION CONNECT_PEER

char options[N_OPTIONS][20] = {
  "Connect to peer",
  "Exit"
};

/**
 * Draws the main menu into window.
 * 
 * @param window Pointer to the ncurses WINDOW to be drawn to
 * @param selected Option to be highlighted in the main menu
*/
void draw_menu(WINDOW* window, MainMenuOption selected) {
  // Loop through the options array and print each option to the screen
  // jumping 1 row each time and highlighting the option if it's selected
  for (int i = 0; i < N_OPTIONS; ++i) {
    // A_STANDOUT == highlight
    if (i == selected) wattron(window, A_STANDOUT);
    else wattroff(window, A_STANDOUT);

    // If the option being drawn is EXIT add it the end of the screen instead
    if (i == EXIT)
      mvwprintw(window, MAIN_MENU_HEIGHT - 2, 1, "0. %-20s", options[i]);
    else
      mvwprintw(window, 3+i, 1, "%d. %-20s", i + 1, options[i]);
  }
  wattroff(window, A_STANDOUT);

  wrefresh(window);
}

/**
 * Selects an option from window.
 * 
 * @param window Pointer to the ncurses window to get input from
 * 
 * @returns The MainMenuOption value for the option the user selected 
*/
MainMenuOption select_item(WINDOW* window) {
  noecho();
  keypad(window, TRUE);
  curs_set(0);

  int key;
  int selected = DEFAULT_OPTION;

  // While user doesn't press Enter
  while ((key = wgetch(window)) != '\n') {
    switch (key) {
      case KEY_DOWN:
        selected++;
        selected = selected % (N_OPTIONS);

        draw_menu(window, selected);
        break;
      case KEY_UP:
        selected--;
        if (selected < 0) selected = (N_OPTIONS - 1);

        draw_menu(window, selected);
        break;
      default:
        break;
    }
  }

  return selected;
}

extern void* menu_thread_func(void* args) {
  MenuThreadArgs typed_args = *(MenuThreadArgs*) args;

  // Always initialize to NO_OPTION since thread function just started
  *typed_args.result = NO_OPTION;

  WINDOW* window = create_window_centered(MAIN_MENU_HEIGHT, MAIN_MENU_WIDTH, true);

  char header[15 + MAX_USERNAME_SIZE];
  snprintf(header, 15 + MAX_USERNAME_SIZE, "Connected as: %s", typed_args.username);
  
  add_title(window, "Main menu");
  add_header(window, header);

  draw_menu(window, DEFAULT_OPTION);
  int selected = select_item(window);

  delwin(window);

  *typed_args.result = selected;

  return NULL;
}