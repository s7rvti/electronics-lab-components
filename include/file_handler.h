/*
 * file_handler.h
 * Saving the inventory to a text file and loading it back safely.
 *
 * File format (plain text):
 *   line 1-6 : available count of each component, one per line
 *   next lines: one issued record per line:  <borrower> <component no.> <spec> <quantity>
 */
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "lab_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Writes the inventory to path (overwrites the file).
   Returns STATUS_OK, STATUS_ERR_NULL_POINTER, STATUS_ERR_FILE_OPEN
   or STATUS_ERR_FILE_WRITE. */
StatusCode saveInventory(const Inventory *inv, const char *path);

/* Reads path into inv. inv is changed only if the whole file is valid.
   Returns STATUS_OK, STATUS_ERR_NULL_POINTER, STATUS_ERR_FILE_OPEN (missing
   file) or STATUS_ERR_FILE_FORMAT (empty, damaged or inconsistent file). */
StatusCode loadInventory(Inventory *inv, const char *path);

#ifdef __cplusplus
}
#endif

#endif
