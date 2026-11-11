/*
 * main.c
 * Lab Component Tracker: start-up, main menu loop and shutdown.
 * All real work is done by the other modules.
 *
 * Usage: ./lab_tracker [data_file]
 */
#include <stdio.h>
#include "lab_types.h"
#include "inventory.h"
#include "file_handler.h"
#include "ui.h"

/* Loads saved data, or starts with full stock if the file is missing or damaged. */
static void loadOrStartFresh(Inventory *inv, const char *dataPath)
{
    StatusCode status = loadInventory(inv, dataPath);

    if (status == STATUS_OK) {
        printf("Loaded saved data from %s\n", dataPath);
        return;
    }
    initInventory(inv);
    if (status == STATUS_ERR_FILE_FORMAT) {
        printf("Warning: %s was empty or damaged. Starting with full stock "
               "(the file will be overwritten on the next save).\n", dataPath);
    } else {
        printf("No saved data found. Starting with full stock.\n");
    }
}

int main(int argc, char *argv[])
{
    Inventory inventory;
    const char *dataPath = (argc > 1) ? argv[1] : DEFAULT_DATA_FILE;
    int choice;
    int running = 1;
    StatusCode status;

    loadOrStartFresh(&inventory, dataPath);

    while (running) {
        printMenu();
        status = readIntegerInRange("Choice", MENU_EXIT, MENU_SAVE_REPORT, &choice);
        if (status == STATUS_ERR_INPUT_CLOSED) {
            break;
        }
        if (status != STATUS_OK) {
            continue;                       /* message already shown by the prompt */
        }

        switch ((MenuChoice)choice) {
        case MENU_ISSUE:       status = handleIssue(&inventory, dataPath);  break;
        case MENU_RETURN:      status = handleReturn(&inventory, dataPath); break;
        case MENU_STOCK:       status = handleViewStock(&inventory);        break;
        case MENU_ISSUED:      status = handleViewIssued(&inventory);       break;
        case MENU_SEARCH:      status = handleSearchBorrower(&inventory);   break;
        case MENU_SPECS:       status = handleViewSpecs();                  break;
        case MENU_SAVE_REPORT: status = handleSaveReport(&inventory);       break;
        case MENU_EXIT:        running = 0;                                 break;
        }
        if (status == STATUS_ERR_INPUT_CLOSED) {
            running = 0;
        }
    }

    status = saveInventory(&inventory, dataPath);
    if (status != STATUS_OK) {
        printf("Warning: could not save data to %s on exit.\n", dataPath);
    }
    printf("Goodbye!\n");
    return 0;
}
