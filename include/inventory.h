/*
 * inventory.h
 * Core logic: what the lab stocks, issuing and returning components,
 * searching, stock status and calculations. No keyboard or file access.
 */
#ifndef INVENTORY_H
#define INVENTORY_H

#include "lab_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- component catalogue (read-only facts about the lab) ---------- */

/* Name such as "Resistor"; "Unknown" for an invalid type. */
const char *getComponentName(ComponentType component);

/* Fixed total quantity the lab owns; 0 for an invalid type. */
int getComponentTotal(ComponentType component);

/* Number of specifications offered (0 for jumper wires or invalid type). */
int getSpecCount(ComponentType component);

/* Specification at 0-based specIndex, or NULL if out of range. */
const char *getSpecName(ComponentType component, int specIndex);

/* Returns 1 if spec is valid for the component (NO_SPEC_LABEL for jumper wires). */
int isKnownSpec(ComponentType component, const char *spec);

/* ---------- operations ---------- */

/* Sets full stock and removes all records. Does nothing if inv is NULL. */
void initInventory(Inventory *inv);

/* Gives 'quantity' items to 'borrower'. specIndex is 0-based (ignored for
   jumper wires). Same borrower + component + spec is merged into one record.
   Returns STATUS_OK, or an error code (nothing changes on error). */
StatusCode issueComponent(Inventory *inv, const char *borrower,
                          ComponentType component, int specIndex, int quantity);

/* Takes back 'quantity' items from the record at recordIndex. The record is
   removed when its quantity reaches zero. */
StatusCode returnComponent(Inventory *inv, int recordIndex, int quantity);

/* ---------- searching ---------- */

/* Writes the record indexes held by borrower for one component into matches
   (at most maxMatches). Returns how many were found (0 if none or bad args). */
int findHeldRecords(const Inventory *inv, const char *borrower,
                    ComponentType component, int matches[], int maxMatches);

/* Same, but across all components. */
int findRecordsByBorrower(const Inventory *inv, const char *borrower,
                          int matches[], int maxMatches);

/* ---------- calculations and status ---------- */

/* Percentage (0-100) of the component currently issued out. */
int calculateUtilisationPercent(const Inventory *inv, ComponentType component);

/* Percentage (0-100) of the component still in the lab. */
int calculateAvailablePercent(const Inventory *inv, ComponentType component);

/* Total number of individual items currently issued. */
int countIssuedItems(const Inventory *inv);

/* STOCK_OUT if none left, STOCK_LOW if at or below LOW_STOCK_PERCENT, else STOCK_OK. */
StockLevel getStockLevel(const Inventory *inv, ComponentType component);

/* Fills order[] with all component types, scarcest first (lowest available %). */
StatusCode sortComponentsByScarcity(const Inventory *inv, ComponentType order[COMP_COUNT]);

#ifdef __cplusplus
}
#endif

#endif
