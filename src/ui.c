#include <stdio.h>
#include "ui.h"
#include "inventory.h"
#include "file_handler.h"
#include "report.h"
#include "validation.h"

/* ====================== messages and low-level input ====================== */

static const char *getStatusMessage(StatusCode status)
{
    switch (status) {
    case STATUS_OK:                     return "Done.";
    case STATUS_ERR_NULL_POINTER:       return "Internal error: missing data.";
    case STATUS_ERR_INVALID_COMPONENT:  return "That component does not exist.";
    case STATUS_ERR_INVALID_SPEC:       return "That specification does not exist.";
    case STATUS_ERR_INVALID_QUANTITY:   return "Invalid quantity.";
    case STATUS_ERR_INVALID_NAME:       return "Invalid name.";
    case STATUS_ERR_INVALID_INPUT:      return "Too many invalid entries. Returning to the menu.";
    case STATUS_ERR_INSUFFICIENT_STOCK: return "Not enough stock available.";
    case STATUS_ERR_CAPACITY_FULL:      return "The record list is full; no new issue can be logged.";
    case STATUS_ERR_NOT_FOUND:          return "No matching record found.";
    case STATUS_ERR_BUFFER_TOO_SMALL:   return "Text is too long.";
    case STATUS_ERR_FILE_OPEN:          return "Could not open the file.";
    case STATUS_ERR_FILE_WRITE:         return "Could not write the file.";
    case STATUS_ERR_FILE_FORMAT:        return "The file contents are invalid.";
    case STATUS_ERR_INPUT_CLOSED:       return "Input ended.";
    }
    return "Unknown error.";
}

/* Reads one line (without the newline) from the keyboard.
   Returns STATUS_ERR_INPUT_CLOSED at end of input and
   STATUS_ERR_INVALID_INPUT if the line was too long for the buffer. */
static StatusCode readLine(char *buffer, int size)
{
    int length;
    int ch;

    if (fgets(buffer, size, stdin) == NULL) {
        return STATUS_ERR_INPUT_CLOSED;
    }
    length = stringLength(buffer);
    if (length > 0 && buffer[length - 1] != '\n') {
        ch = getchar();
        if (ch != EOF) {                       /* line did not fit: discard the rest */
            while (ch != '\n' && ch != EOF) {
                ch = getchar();
            }
            return STATUS_ERR_INVALID_INPUT;
        }
    }
    trimNewline(buffer);
    return STATUS_OK;
}

StatusCode readIntegerInRange(const char *prompt, int min, int max, int *value)
{
    char line[INPUT_LINE_SIZE];
    StatusCode status;
    int number;
    int attempt;

    if (prompt == NULL || value == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    for (attempt = 0; attempt < MAX_INPUT_ATTEMPTS; attempt++) {
        printf("%s (%d-%d): ", prompt, min, max);
        status = readLine(line, INPUT_LINE_SIZE);
        if (status == STATUS_ERR_INPUT_CLOSED) {
            return status;
        }
        if (status == STATUS_OK) {
            status = parseInteger(line, &number);
        }
        if (status == STATUS_OK && isInRange(number, min, max)) {
            *value = number;
            return STATUS_OK;
        }
        printf("Invalid input. Please type a whole number from %d to %d.\n", min, max);
    }
    return STATUS_ERR_INVALID_INPUT;
}

/* Asks for a borrower name until it is valid (same retry rule as numbers). */
static StatusCode readName(const char *prompt, char *name, int size)
{
    char line[INPUT_LINE_SIZE];
    StatusCode status;
    int attempt;

    for (attempt = 0; attempt < MAX_INPUT_ATTEMPTS; attempt++) {
        printf("%s", prompt);
        status = readLine(line, INPUT_LINE_SIZE);
        if (status == STATUS_ERR_INPUT_CLOSED) {
            return status;
        }
        if (status == STATUS_OK && isValidName(line)) {
            return copyString(name, size, line);
        }
        printf("Invalid name. Use 1-%d letters, digits, '_' or '-' (no spaces).\n",
               NAME_MAX_CHARS);
    }
    return STATUS_ERR_INVALID_INPUT;
}

/* ====================== shared prompts ====================== */

/* Saves the inventory and warns on failure. Returns the save status. */
static StatusCode saveAfterChange(const Inventory *inv, const char *dataPath)
{
    StatusCode status = saveInventory(inv, dataPath);

    if (status != STATUS_OK) {
        printf("Warning: changes were NOT saved. %s\n", getStatusMessage(status));
    }
    return status;
}

static void printComponentList(const Inventory *inv)
{
    int i;

    printf("\nComponents:\n");
    for (i = 0; i < COMP_COUNT; i++) {
        printf("  %d. %-13s (available: %d)\n", i + 1,
               getComponentName((ComponentType)i), inv->available[i]);
    }
}

static void printSpecList(ComponentType component)
{
    int i;

    printf("\nAvailable %s types:\n", getComponentName(component));
    for (i = 0; i < getSpecCount(component); i++) {
        printf("  %d. %s\n", i + 1, getSpecName(component, i));
    }
}

/* Shows the component list and asks for one. */
static StatusCode promptComponent(const Inventory *inv, ComponentType *component)
{
    int choice;
    StatusCode status;

    printComponentList(inv);
    status = readIntegerInRange("Choose component", 1, COMP_COUNT, &choice);
    if (status == STATUS_OK) {
        *component = (ComponentType)(choice - 1);
    }
    return status;
}

/* Shows the specification list and asks for one (0-based index).
   Jumper wires have no specification, so nothing is asked for them. */
static StatusCode promptSpec(ComponentType component, int *specIndex)
{
    int choice;
    StatusCode status;

    if (component == COMP_JUMPER_WIRE) {
        *specIndex = 0;
        return STATUS_OK;
    }
    printSpecList(component);
    status = readIntegerInRange("Choose type", 1, getSpecCount(component), &choice);
    if (status == STATUS_OK) {
        *specIndex = choice - 1;
    }
    return status;
}

/* ====================== menu ====================== */

void printMenu(void)
{
    printf("\n===== LAB COMPONENT LOG =====\n");
    printf("%d. Take a component\n", MENU_ISSUE);
    printf("%d. Return a component\n", MENU_RETURN);
    printf("%d. View stock report\n", MENU_STOCK);
    printf("%d. View issued components\n", MENU_ISSUED);
    printf("%d. Search by borrower name\n", MENU_SEARCH);
    printf("%d. View all component types/specifications\n", MENU_SPECS);
    printf("%d. Save report to file\n", MENU_SAVE_REPORT);
    printf("%d. Exit\n", MENU_EXIT);
}

/* ====================== handlers ====================== */

StatusCode handleIssue(Inventory *inv, const char *dataPath)
{
    char borrower[NAME_SIZE];
    ComponentType component;
    int specIndex;
    int quantity;
    StatusCode status;

    status = readName("Enter your name: ", borrower, NAME_SIZE);
    if (status == STATUS_OK) {
        status = promptComponent(inv, &component);
    }
    if (status == STATUS_OK && inv->available[component] == 0) {
        printf("Sorry, no %s left in the lab.\n", getComponentName(component));
        return STATUS_ERR_INSUFFICIENT_STOCK;
    }
    if (status == STATUS_OK) {
        status = promptSpec(component, &specIndex);
    }
    if (status == STATUS_OK) {
        status = readIntegerInRange("Enter quantity", 1, inv->available[component],
                                    &quantity);
    }
    if (status == STATUS_OK) {
        status = issueComponent(inv, borrower, component, specIndex, quantity);
    }
    if (status != STATUS_OK) {
        printf("%s\n", getStatusMessage(status));
        return status;
    }

    printf("Issued %d x %s (%s) to %s.\n", quantity, getComponentName(component),
           component == COMP_JUMPER_WIRE ? NO_SPEC_LABEL : getSpecName(component, specIndex),
           borrower);
    if (getStockLevel(inv, component) != STOCK_OK) {
        printf("Warning: only %d %s left in the lab.\n", inv->available[component],
               getComponentName(component));
    }
    return saveAfterChange(inv, dataPath);
}

StatusCode handleReturn(Inventory *inv, const char *dataPath)
{
    char borrower[NAME_SIZE];
    int matches[MAX_RECORDS];
    ComponentType component;
    int found;
    int pick = 1;
    int quantity;
    int i;
    StatusCode status;

    status = readName("Enter your name: ", borrower, NAME_SIZE);
    if (status == STATUS_OK) {
        status = promptComponent(inv, &component);
    }
    if (status != STATUS_OK) {
        printf("%s\n", getStatusMessage(status));
        return status;
    }

    /* Stop here if this person never took this component. */
    found = findHeldRecords(inv, borrower, component, matches, MAX_RECORDS);
    if (found == 0) {
        printf("\n%s, you have not issued any %s. Nothing to return.\n", borrower,
               getComponentName(component));
        return STATUS_ERR_NOT_FOUND;
    }
    if (found > 1) {
        printf("\nYou currently hold these %s items:\n", getComponentName(component));
        for (i = 0; i < found; i++) {
            printf("  %d. %s  (qty: %d)\n", i + 1, inv->records[matches[i]].spec,
                   inv->records[matches[i]].quantity);
        }
        status = readIntegerInRange("Which one are you returning", 1, found, &pick);
    }
    if (status == STATUS_OK) {
        printf("You hold %d x %s (%s).\n", inv->records[matches[pick - 1]].quantity,
               getComponentName(component), inv->records[matches[pick - 1]].spec);
        status = readIntegerInRange("How many are you returning", 1,
                                    inv->records[matches[pick - 1]].quantity, &quantity);
    }
    if (status == STATUS_OK) {
        status = returnComponent(inv, matches[pick - 1], quantity);
    }
    if (status != STATUS_OK) {
        printf("%s\n", getStatusMessage(status));
        return status;
    }

    printf("Returned %d x %s. Thank you, %s!\n", quantity, getComponentName(component),
           borrower);
    return saveAfterChange(inv, dataPath);
}

StatusCode handleViewStock(const Inventory *inv)
{
    return writeStockReport(inv, stdout);
}

StatusCode handleViewIssued(const Inventory *inv)
{
    return writeIssuedReport(inv, stdout);
}

StatusCode handleSearchBorrower(const Inventory *inv)
{
    char borrower[NAME_SIZE];
    StatusCode status = readName("Enter the borrower name to search: ", borrower, NAME_SIZE);

    if (status != STATUS_OK) {
        printf("%s\n", getStatusMessage(status));
        return status;
    }
    return writeBorrowerReport(inv, borrower, stdout);
}

StatusCode handleViewSpecs(void)
{
    int i;

    for (i = 0; i < COMP_COUNT; i++) {
        if (getSpecCount((ComponentType)i) > 0) {
            printSpecList((ComponentType)i);
        }
    }
    printf("\n%s: no specification needed.\n", getComponentName(COMP_JUMPER_WIRE));
    return STATUS_OK;
}

StatusCode handleSaveReport(const Inventory *inv)
{
    StatusCode status = saveReportToFile(inv, DEFAULT_REPORT_FILE);

    if (status == STATUS_OK) {
        printf("Report saved to %s\n", DEFAULT_REPORT_FILE);
    } else {
        printf("Could not save the report. %s\n", getStatusMessage(status));
    }
    return status;
}
