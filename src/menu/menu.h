#ifndef SRC_MENU_H
#define SRC_MENU_H

/**
 * @file menu.h
 * @brief Interactive command-line menu for the register allocation tool.
 */

/**
 * @brief Launches the interactive command-line menu.
 *
 * Presents the user with options to load the ranges and registers input files,
 * run the selected allocation algorithm, and save the results to an output file.
 * Loops until the user chooses to exit.
 */
void runMenu();

#endif //SRC_MENU_H
