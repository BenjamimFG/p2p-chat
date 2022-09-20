#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <ncurses.h>

#include "p2p_chat.h"

// Width and Height of main menu
#define MAIN_MENU_WIDTH 36
#define MAIN_MENU_HEIGHT 14

typedef enum _main_menu_option {
  NO_OPTION = -1,
  CONNECT_PEER,
  EXIT
} MainMenuOption;

typedef struct _menu_thread_args {
  volatile MainMenuOption* result;
  char username[MAX_USERNAME_SIZE + 1];
} MenuThreadArgs;

/**
 * Thread function for the main menu thread. Draws the main menu
 * and waits till the user selects an option. Sets the args->result
 * to the selected option.
 * 
 * @param args Arguments should actually be of MenuThreadArgs* type
 * 
 * @returns NULL
*/
extern void* menu_thread_func(void* args);

#endif