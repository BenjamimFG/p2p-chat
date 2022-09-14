#ifndef CONNECT_WINDOW_H
#define CONNECT_WINDOW_H

#include <ncurses.h>

/**
 * Draws the window for the user to type the ip and port to connect to
 * 
 * @param window ncurses WINDOW to draw to
*/
extern void draw_connect_window(WINDOW* window);

/**
 * Reads a 21 length string from the user and returns a pointer to it.
 * 
 * @param window Pointer to the ncurses WINDOW to get the string from
 * 
 * @returns A pointer to the string read
*/
extern char* get_connect_ip_and_port(WINDOW* window);

#endif