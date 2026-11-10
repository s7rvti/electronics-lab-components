#include <stdio.h>
#include "inventory.h"
#include "validation.h"

#define UNKNOWN_COMPONENT_NAME "Unknown"

/* Fixed facts about each component the lab stocks.
   Specifications end at the first NULL (jumper wires have none).
   To change what the lab owns, edit this table only. */
typedef struct {
    const char *name;
    int         total;
    const char *specs[MAX_SPECS_PER_COMPONENT];
} ComponentInfo;

static const ComponentInfo CATALOGUE[COMP_COUNT] = {
    {"Resistor", 100,
        {"100", "220", "470", "1k", "4.7k", "10k", "47k", "100k", "1M", NULL}},
    {"Capacitor", 100,
        {"100pF", "10nF", "100nF", "1uF", "10uF", "100uF", "1000uF", NULL}},
    {"Inductor", 30,
        {"10uH", "100uH", "1mH", "10mH", "100mH", NULL}},
    {"Transistor", 50,
        {"BC547", "BC557", "2N2222", "2N3904", "2N3055", NULL}},
    {"MOSFET", 30,
        {"IRF540", "IRF9540", "IRFZ44N", "2N7000", "IRLZ44N", NULL}},
    {"Jumper wires", 200,
        {NULL}}
};

/* ====================== catalogue ====================== */

const char *getComponentName(ComponentType component)
{
    if (!isValidComponent(component)) {
        return UNKNOWN_COMPONENT_NAME;
    }
    return CATALOGUE[component].name;
}

int getComponentTotal(ComponentType component)
{
    if (!isValidComponent(component)) {
        return 0;
    }
    return CATALOGUE[component].total;
}

int getSpecCount(ComponentType component)
{
    int count = 0;

    if (!isValidComponent(component)) {
        return 0;
    }
    while (count < MAX_SPECS_PER_COMPONENT &&
           CATALOGUE[component].specs[count] != NULL) {
        count++;
    }
    return count;
}

const char *getSpecName(ComponentType component, int specIndex)
{
    if (!isInRange(specIndex, 0, getSpecCount(component) - 1)) {
        return NULL;
    }
    return CATALOGUE[component].specs[specIndex];
}

int isKnownSpec(ComponentType component, const char *spec)
{
    int i;

    if (!isValidComponent(component) || spec == NULL) {
        return 0;
    }
    if (component == COMP_JUMPER_WIRE) {
        return stringsEqual(spec, NO_SPEC_LABEL);
    }
    for (i = 0; i < getSpecCount(component); i++) {
        if (stringsEqual(spec, CATALOGUE[component].specs[i])) {
            return 1;
        }
    }
    return 0;
}

/* ====================== helpers (private) ====================== */

/* Returns the specification text to store, or NULL if specIndex is invalid. */
static const char *resolveSpec(ComponentType component, int specIndex)
{
    if (component == COMP_JUMPER_WIRE) {
        return NO_SPEC_LABEL;
    }
    return getSpecName(component, specIndex);
}

/* Returns the index of the record with this exact borrower/component/spec, or -1. */
static int findExactRecord(const Inventory *inv, const char *borrower,
                           ComponentType component, const char *spec)
{
    int i;

    for (i = 0; i < inv->recordCount; i++) {
        if (inv->records[i].component == component &&
            stringsEqual(inv->records[i].borrower, borrower) &&
            stringsEqual(inv->records[i].spec, spec)) {
            return i;
        }
    }
    return -1;
}

/* Deletes a record by shifting the later records up by one place. */
static void removeRecord(Inventory *inv, int recordIndex)
{
    int i;

    for (i = recordIndex; i < inv->recordCount - 1; i++) {
        inv->records[i] = inv->records[i + 1];
    }
    inv->recordCount--;
}

/* ====================== operations ====================== */

void initInventory(Inventory *inv)
{
    int i;

    if (inv == NULL) {
        return;
    }
    for (i = 0; i < COMP_COUNT; i++) {
        inv->available[i] = CATALOGUE[i].total;
    }
    inv->recordCount = 0;
}

StatusCode issueComponent(Inventory *inv, const char *borrower,
                          ComponentType component, int specIndex, int quantity)
{
    const char *spec;
    IssueRecord *record;
    StatusCode status;
    int existing;

    if (inv == NULL || borrower == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    if (!isValidComponent(component)) {
        return STATUS_ERR_INVALID_COMPONENT;
    }
    if (!isValidName(borrower)) {
        return STATUS_ERR_INVALID_NAME;
    }
    spec = resolveSpec(component, specIndex);
    if (spec == NULL) {
        return STATUS_ERR_INVALID_SPEC;
    }
    if (quantity <= 0) {
        return STATUS_ERR_INVALID_QUANTITY;
    }
    if (quantity > inv->available[component]) {
        return STATUS_ERR_INSUFFICIENT_STOCK;
    }

    existing = findExactRecord(inv, borrower, component, spec);
    if (existing >= 0) {
        inv->records[existing].quantity += quantity;
    } else {
        if (inv->recordCount >= MAX_RECORDS) {
            return STATUS_ERR_CAPACITY_FULL;
        }
        record = &inv->records[inv->recordCount];
        status = copyString(record->borrower, NAME_SIZE, borrower);
        if (status == STATUS_OK) {
            status = copyString(record->spec, SPEC_SIZE, spec);
        }
        if (status != STATUS_OK) {
            return status;      /* recordCount not increased: nothing changed */
        }
        record->component = component;
        record->quantity = quantity;
        inv->recordCount++;
    }
    inv->available[component] -= quantity;
    return STATUS_OK;
}

StatusCode returnComponent(Inventory *inv, int recordIndex, int quantity)
{
    IssueRecord *record;

    if (inv == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    if (!isInRange(recordIndex, 0, inv->recordCount - 1)) {
        return STATUS_ERR_NOT_FOUND;
    }
    record = &inv->records[recordIndex];
    if (!isInRange(quantity, 1, record->quantity)) {
        return STATUS_ERR_INVALID_QUANTITY;
    }

    inv->available[record->component] += quantity;
    record->quantity -= quantity;
    if (record->quantity == 0) {
        removeRecord(inv, recordIndex);
    }
    return STATUS_OK;
}

/* ====================== searching ====================== */

int findHeldRecords(const Inventory *inv, const char *borrower,
                    ComponentType component, int matches[], int maxMatches)
{
    int i;
    int found = 0;

    if (inv == NULL || borrower == NULL || matches == NULL) {
        return 0;
    }
    for (i = 0; i < inv->recordCount && found < maxMatches; i++) {
        if (inv->records[i].component == component &&
            stringsEqual(inv->records[i].borrower, borrower)) {
            matches[found] = i;
            found++;
        }
    }
    return found;
}

int findRecordsByBorrower(const Inventory *inv, const char *borrower,
                          int matches[], int maxMatches)
{
    int i;
    int found = 0;

    if (inv == NULL || borrower == NULL || matches == NULL) {
        return 0;
    }
    for (i = 0; i < inv->recordCount && found < maxMatches; i++) {
        if (stringsEqual(inv->records[i].borrower, borrower)) {
            matches[found] = i;
            found++;
        }
    }
    return found;
}

/* ====================== calculations and status ====================== */

int calculateUtilisationPercent(const Inventory *inv, ComponentType component)
{
    int total = getComponentTotal(component);

    if (inv == NULL || total == 0) {
        return 0;
    }
    return (total - inv->available[component]) * PERCENT_FULL / total;
}

int calculateAvailablePercent(const Inventory *inv, ComponentType component)
{
    int total = getComponentTotal(component);

    if (inv == NULL || total == 0) {
        return 0;
    }
    return inv->available[component] * PERCENT_FULL / total;
}

int countIssuedItems(const Inventory *inv)
{
    int i;
    int total = 0;

    if (inv == NULL) {
        return 0;
    }
    for (i = 0; i < inv->recordCount; i++) {
        total += inv->records[i].quantity;
    }
    return total;
}

StockLevel getStockLevel(const Inventory *inv, ComponentType component)
{
    int total = getComponentTotal(component);

    if (inv == NULL || total == 0 || inv->available[component] <= 0) {
        return STOCK_OUT;
    }
    /* Compare without division so the 20% boundary is exact. */
    if (inv->available[component] * PERCENT_FULL <= total * LOW_STOCK_PERCENT) {
        return STOCK_LOW;
    }
    return STOCK_OK;
}

StatusCode sortComponentsByScarcity(const Inventory *inv, ComponentType order[COMP_COUNT])
{
    int i;
    int j;
    ComponentType current;

    if (inv == NULL || order == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    for (i = 0; i < COMP_COUNT; i++) {
        order[i] = (ComponentType)i;
    }
    /* Insertion sort: stable, so equal percentages keep catalogue order. */
    for (i = 1; i < COMP_COUNT; i++) {
        current = order[i];
        j = i - 1;
        while (j >= 0 && calculateAvailablePercent(inv, order[j]) >
                         calculateAvailablePercent(inv, current)) {
            order[j + 1] = order[j];
            j--;
        }
        order[j + 1] = current;
    }
    return STATUS_OK;
}
