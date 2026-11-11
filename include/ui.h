/*
 * ui.h
 * Everything that talks to the user: menu, prompts, and the interactive
 * take / return / search / report screens. Handlers save the data file
 * after every successful change.
 */
#ifndef UI_H
#define UI_H

#include "lab_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INPUT_LINE_SIZE    64
#define MAX_INPUT_ATTEMPTS 3     /* wrong answers allowed before a prompt gives up */

typedef enum {
    MENU_EXIT = 0,
    MENU_ISSUE,
    MENU_RETURN,
    MENU_STOCK,
    MENU_ISSUED,
    MENU_SEARCH,
    MENU_SPECS,
    MENU_SAVE_REPORT
} MenuChoice;

/* Prints the main menu. */
void printMenu(void);

/* Prompts "<prompt> (min-max): " until a whole number in range is typed.
   Gives up after MAX_INPUT_ATTEMPTS wrong answers (STATUS_ERR_INVALID_INPUT)
   or when input ends (STATUS_ERR_INPUT_CLOSED). */
StatusCode readIntegerInRange(const char *prompt, int min, int max, int *value);

/* Each handler runs one menu action and returns its final status.
   dataPath is the inventory file to save to after a change. */
StatusCode handleIssue(Inventory *inv, const char *dataPath);
StatusCode handleReturn(Inventory *inv, const char *dataPath);
StatusCode handleViewStock(const Inventory *inv);
StatusCode handleViewIssued(const Inventory *inv);
StatusCode handleSearchBorrower(const Inventory *inv);
StatusCode handleViewSpecs(void);
StatusCode handleSaveReport(const Inventory *inv);

#ifdef __cplusplus
}
#endif

#endif
