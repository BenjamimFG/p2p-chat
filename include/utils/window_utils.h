#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H

#include <stdbool.h>
#include <ncurses.h>

/**
 * Adds a string title to the ncurses window.
 * The title is added at position (0, 2), so it 'cuts' through the box if the window has one.
 * 
 * @param window Nucrses window to add the title to
 * @param title Title to be added to window
*/
void add_title(WINDOW* window, const char* title);

/**
 * Creates a Ncurses WINDOW centered in the screen and returns a pointer to it.
 * Window can be boxed or not depending on is_boxed param.
 * 
 * @param height Height of the window
 * @param width Width of the window
 * @param is_boxed Window will have box(window, 0, 0) called if this parameter is true
 * 
 * @returns Pointer to the window
*/
WINDOW* create_window_centered(const int height, const int width, const bool is_boxed);

/**
 * Adds a header and refreshes the ncurses window.
 * The header string is added at position 1,1 of the window.
 * 
 * @param window Pointer to the ncurses window to add the header to
 * @param header Pointer to the string containing the header to be added
*/
extern void add_header(WINDOW* window, const char* header);

/**
 * Shows prompt at position start_y,start_x and gets a string of size n from user.
 * This function sets echo() and curs_set(1).
 * 
 * @param window Pointer to the ncurses window to get the string from
 * @param n Size of the string to read from user
 * @param prompt Prompt to be shown to the user
 * @param start_y Y position to show prompt at
 * @param start_x X position to show prompt at
 * 
 * @returns Pointer to string of size n allocated on the heap (The returning
 * pointer needs to be freed when done using it)
*/
extern char* get_string(WINDOW* window, const int n, const char* prompt,
                        const int start_y, const int start_x);

/**
 * Shows prompt at position start_y,start_x and gets a char from user. Returns
 * true if the char pressed was 'y' or 'Y', false otherwise.
 * This function sets noecho() and curs_set(0).
 * 
 * @param window Pointer to the ncurses window to get the char from
 * @param prompt Prompt to be shown to the user
 * @param start_y Y position to show prompt at
 * @param start_x X position to show prompt at
 * 
 * @returns true if the user presses 'y' or 'Y', false otherwise
*/
extern bool get_y_n(WINDOW* window, const char* prompt, const int start_y,
                    const int start_x);

/**
 * Calls clear() and refresh() immediately after.
*/
extern void clear_screen();

#endif