#ifndef WINDOW_UTILS_H
#define WINDOW_UTILS_H

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
 * 
 * @param height Height of the window
 * @param width Width of the window
 * 
 * @returns Pointer to the window
*/
WINDOW* create_window_centered(const int height, const int width);

#endif