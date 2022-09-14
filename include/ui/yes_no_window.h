#ifndef YES_NO_WINDOW_H
#define YES_NO_WINDOW_H

#include <ncurses.h>
#include <stdbool.h>

/**
 * Draws a question on the window and appends the string "[y/N]" to it.
 * 
 * @param window Pointer to the ncurses WINDOW to be drawn
 * @param question Pointer to the string with the question to be printed to the window
*/
void draw_yes_no_window(WINDOW* window, const char* question);

/**
 * Gets a character from window and returns true or false depending on the user answer.
 * 
 * @param window Pointer to the ncurses WINDOW to get the character from
 * 
 * @returns true if the user presses 'y' or 'Y', false otherwise
*/
bool get_answer(WINDOW* window);

#endif