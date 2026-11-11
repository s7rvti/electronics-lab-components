/*
 * report.h
 * Builds the text reports. Every writer takes a FILE*, so the same code
 * prints to the screen (stdout), to a report file, or to a test buffer.
 */
#ifndef REPORT_H
#define REPORT_H

#include <stdio.h>
#include "lab_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Text for a stock level: "OUT OF STOCK", "LOW" or "OK". */
const char *getStockLevelLabel(StockLevel level);

/* Stock table: total, available, issued, utilisation % and level for every
   component, scarcest first, followed by a summary and restock warnings. */
StatusCode writeStockReport(const Inventory *inv, FILE *out);

/* Every issued record (who holds what), or a message if nothing is issued. */
StatusCode writeIssuedReport(const Inventory *inv, FILE *out);

/* Everything one borrower currently holds, or a message if nothing. */
StatusCode writeBorrowerReport(const Inventory *inv, const char *borrower, FILE *out);

/* Writes the stock report and the issued report into the file at path. */
StatusCode saveReportToFile(const Inventory *inv, const char *path);

#ifdef __cplusplus
}
#endif

#endif
