#include <stdio.h>
#include "file_handler.h"
#include "inventory.h"
#include "validation.h"

#define LINE_SIZE 128

/* Builds "%29s" style widths from the size constants, so scanf cannot overflow. */
#define STRINGIFY_VALUE(x) #x
#define STRINGIFY(x)       STRINGIFY_VALUE(x)
#define RECORD_FORMAT "%" STRINGIFY(NAME_MAX_CHARS) "s %d %" STRINGIFY(SPEC_MAX_CHARS) "s %d %c"
#define FIELDS_PER_RECORD 4

/* ====================== saving ====================== */

StatusCode saveInventory(const Inventory *inv, const char *path)
{
    FILE *file;
    int i;
    int failed = 0;

    if (inv == NULL || path == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    file = fopen(path, "w");
    if (file == NULL) {
        return STATUS_ERR_FILE_OPEN;
    }
    for (i = 0; i < COMP_COUNT; i++) {
        failed |= (fprintf(file, "%d\n", inv->available[i]) < 0);
    }
    for (i = 0; i < inv->recordCount; i++) {
        failed |= (fprintf(file, "%s %d %s %d\n", inv->records[i].borrower,
                           (int)inv->records[i].component, inv->records[i].spec,
                           inv->records[i].quantity) < 0);
    }
    if (fclose(file) != 0) {
        failed = 1;
    }
    return failed ? STATUS_ERR_FILE_WRITE : STATUS_OK;
}

/* ====================== loading (private helpers) ====================== */

/* Reads the first COMP_COUNT lines: the available count of each component. */
static StatusCode readCounts(FILE *file, Inventory *inv)
{
    char line[LINE_SIZE];
    int i;
    int value;

    for (i = 0; i < COMP_COUNT; i++) {
        if (fgets(line, LINE_SIZE, file) == NULL) {
            return STATUS_ERR_FILE_FORMAT;
        }
        trimNewline(line);
        if (parseInteger(line, &value) != STATUS_OK ||
            !isInRange(value, 0, getComponentTotal((ComponentType)i))) {
            return STATUS_ERR_FILE_FORMAT;
        }
        inv->available[i] = value;
    }
    return STATUS_OK;
}

/* Parses and validates one record line into record. */
static StatusCode parseRecordLine(const char *line, IssueRecord *record)
{
    char borrower[NAME_SIZE];
    char spec[SPEC_SIZE];
    char extra;
    int component;
    int quantity;

    if (sscanf(line, RECORD_FORMAT, borrower, &component, spec, &quantity,
               &extra) != FIELDS_PER_RECORD) {
        return STATUS_ERR_FILE_FORMAT;
    }
    if (!isValidName(borrower) || !isValidComponent(component) ||
        !isKnownSpec((ComponentType)component, spec) ||
        !isInRange(quantity, 1, getComponentTotal((ComponentType)component))) {
        return STATUS_ERR_FILE_FORMAT;
    }
    if (copyString(record->borrower, NAME_SIZE, borrower) != STATUS_OK ||
        copyString(record->spec, SPEC_SIZE, spec) != STATUS_OK) {
        return STATUS_ERR_FILE_FORMAT;
    }
    record->component = (ComponentType)component;
    record->quantity = quantity;
    return STATUS_OK;
}

/* Reads every remaining line as an issued record (blank lines are skipped). */
static StatusCode readRecords(FILE *file, Inventory *inv)
{
    char line[LINE_SIZE];
    StatusCode status;

    inv->recordCount = 0;
    while (fgets(line, LINE_SIZE, file) != NULL) {
        trimNewline(line);
        if (stringLength(line) == 0) {
            continue;
        }
        if (inv->recordCount >= MAX_RECORDS) {
            return STATUS_ERR_FILE_FORMAT;
        }
        status = parseRecordLine(line, &inv->records[inv->recordCount]);
        if (status != STATUS_OK) {
            return status;
        }
        inv->recordCount++;
    }
    return STATUS_OK;
}

/* For each component, available + issued must equal the fixed total. */
static StatusCode checkConsistency(const Inventory *inv)
{
    int issued[COMP_COUNT] = {0};
    int i;

    for (i = 0; i < inv->recordCount; i++) {
        issued[inv->records[i].component] += inv->records[i].quantity;
    }
    for (i = 0; i < COMP_COUNT; i++) {
        if (inv->available[i] + issued[i] != getComponentTotal((ComponentType)i)) {
            return STATUS_ERR_FILE_FORMAT;
        }
    }
    return STATUS_OK;
}

/* ====================== loading ====================== */

StatusCode loadInventory(Inventory *inv, const char *path)
{
    Inventory loaded;
    StatusCode status;
    FILE *file;

    if (inv == NULL || path == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    file = fopen(path, "r");
    if (file == NULL) {
        return STATUS_ERR_FILE_OPEN;
    }

    initInventory(&loaded);
    status = readCounts(file, &loaded);
    if (status == STATUS_OK) {
        status = readRecords(file, &loaded);
    }
    if (fclose(file) != 0 && status == STATUS_OK) {
        status = STATUS_ERR_FILE_OPEN;
    }

    if (status == STATUS_OK) {
        status = checkConsistency(&loaded);
    }
    if (status == STATUS_OK) {
        *inv = loaded;              /* only replace the caller's data when all is valid */
    }
    return status;
}
