#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <ncurses.h>
#include <signal.h>

#include "p2p_server.h"

// Width and Height
#define MAIN_MENU_WIDTH 36
#define MAIN_MENU_HEIGHT 14

// Number of options in the menu
#define N_OPTIONS 2

typedef enum _main_menu_option {
  NO_OPTION = -1,
  CONNECT_PEER,
  EXIT
} MainMenuOption;

typedef struct _menu_thread_args {
  WINDOW* window;
  char username[MAX_USERNAME_SIZE];
  volatile sig_atomic_t* result;
} MenuThreadArgs;

void draw_menu(WINDOW* window, char* username);
int select_item(WINDOW* window, char* username);
void* menu_thread_func(void* args);

#endif