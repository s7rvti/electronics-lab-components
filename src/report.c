#include <stdio.h>
#include "report.h"
#include "inventory.h"
#include "validation.h"

const char *getStockLevelLabel(StockLevel level)
{
    if (level == STOCK_OUT) {
        return "OUT OF STOCK";
    }
    if (level == STOCK_LOW) {
        return "LOW";
    }
    return "OK";
}

/* Prints the heading and one row for record number recordIndex. */
static int writeRecordRow(const Inventory *inv, int recordIndex, FILE *out)
{
    const IssueRecord *record = &inv->records[recordIndex];

    return fprintf(out, "%-*s %-14s %-10s %d\n", NAME_MAX_CHARS, record->borrower,
                   getComponentName(record->component), record->spec,
                   record->quantity) < 0;
}

static int writeRecordHeading(FILE *out)
{
    return fprintf(out, "%-*s %-14s %-10s %s\n", NAME_MAX_CHARS, "Borrower",
                   "Component", "Spec", "Qty") < 0;
}

StatusCode writeStockReport(const Inventory *inv, FILE *out)
{
    ComponentType order[COMP_COUNT];
    ComponentType component;
    StockLevel level;
    int attention = 0;
    int failed = 0;
    int i;

    if (inv == NULL || out == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    if (sortComponentsByScarcity(inv, order) != STATUS_OK) {
        return STATUS_ERR_NULL_POINTER;
    }

    failed |= (fprintf(out, "\n===== STOCK REPORT (scarcest first) =====\n") < 0);
    failed |= (fprintf(out, "%-14s %6s %10s %8s %6s  %s\n", "Component", "Total",
                       "Available", "Issued", "Used%", "Status") < 0);
    for (i = 0; i < COMP_COUNT; i++) {
        component = order[i];
        level = getStockLevel(inv, component);
        failed |= (fprintf(out, "%-14s %6d %10d %8d %5d%%  %s\n",
                           getComponentName(component), getComponentTotal(component),
                           inv->available[component],
                           getComponentTotal(component) - inv->available[component],
                           calculateUtilisationPercent(inv, component),
                           getStockLevelLabel(level)) < 0);
        if (level != STOCK_OK) {
            attention++;
        }
    }
    failed |= (fprintf(out, "\nItems currently issued: %d\n", countIssuedItems(inv)) < 0);
    if (attention > 0) {
        failed |= (fprintf(out, "Restock needed for %d component type(s).\n", attention) < 0);
    } else {
        failed |= (fprintf(out, "All components are sufficiently stocked.\n") < 0);
    }
    return failed ? STATUS_ERR_FILE_WRITE : STATUS_OK;
}

StatusCode writeIssuedReport(const Inventory *inv, FILE *out)
{
    int failed = 0;
    int i;

    if (inv == NULL || out == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    failed |= (fprintf(out, "\n===== ISSUED COMPONENTS =====\n") < 0);
    if (inv->recordCount == 0) {
        failed |= (fprintf(out, "Nothing is currently issued.\n") < 0);
        return failed ? STATUS_ERR_FILE_WRITE : STATUS_OK;
    }
    failed |= writeRecordHeading(out);
    for (i = 0; i < inv->recordCount; i++) {
        failed |= writeRecordRow(inv, i, out);
    }
    return failed ? STATUS_ERR_FILE_WRITE : STATUS_OK;
}

StatusCode writeBorrowerReport(const Inventory *inv, const char *borrower, FILE *out)
{
    int matches[MAX_RECORDS];
    int found;
    int failed = 0;
    int i;

    if (inv == NULL || borrower == NULL || out == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    found = findRecordsByBorrower(inv, borrower, matches, MAX_RECORDS);
    if (found == 0) {
        failed |= (fprintf(out, "\n%s currently holds nothing.\n", borrower) < 0);
        return failed ? STATUS_ERR_FILE_WRITE : STATUS_OK;
    }
    failed |= (fprintf(out, "\n===== ITEMS HELD BY %s =====\n", borrower) < 0);
    failed |= writeRecordHeading(out);
    for (i = 0; i < found; i++) {
        failed |= writeRecordRow(inv, matches[i], out);
    }
    return failed ? STATUS_ERR_FILE_WRITE : STATUS_OK;
}

StatusCode saveReportToFile(const Inventory *inv, const char *path)
{
    FILE *file;
    StatusCode status;

    if (inv == NULL || path == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    file = fopen(path, "w");
    if (file == NULL) {
        return STATUS_ERR_FILE_OPEN;
    }
    status = writeStockReport(inv, file);
    if (status == STATUS_OK) {
        status = writeIssuedReport(inv, file);
    }
    if (fclose(file) != 0 && status == STATUS_OK) {
        status = STATUS_ERR_FILE_WRITE;
    }
    return status;
}
