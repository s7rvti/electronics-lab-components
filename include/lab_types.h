/*
 * lab_types.h
 * Shared constants, enumerations and structures used by every module.
 */
#ifndef LAB_TYPES_H
#define LAB_TYPES_H

/* ---------- size limits ---------- */
#define NAME_MAX_CHARS           29                    /* longest borrower name   */
#define NAME_SIZE                (NAME_MAX_CHARS + 1)  /* buffer size incl. '\0'  */
#define SPEC_MAX_CHARS           19                    /* longest specification   */
#define SPEC_SIZE                (SPEC_MAX_CHARS + 1)
#define MAX_RECORDS              100                   /* max simultaneous issues */
#define MAX_SPECS_PER_COMPONENT  10

/* ---------- rules and labels ---------- */
#define PERCENT_FULL             100
#define LOW_STOCK_PERCENT        20                    /* at or below this = LOW  */
#define NO_SPEC_LABEL            "-"                   /* used when a component has no specification */
#define DEFAULT_DATA_FILE        "data/inventory.txt"
#define DEFAULT_REPORT_FILE      "data/stock_report.txt"

/* The component kinds the lab stocks. COMP_COUNT is the number of kinds. */
typedef enum {
    COMP_RESISTOR = 0,
    COMP_CAPACITOR,
    COMP_INDUCTOR,
    COMP_TRANSISTOR,
    COMP_MOSFET,
    COMP_JUMPER_WIRE,
    COMP_COUNT
} ComponentType;

/* Result of every operation that can fail. */
typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL_POINTER,
    STATUS_ERR_INVALID_COMPONENT,
    STATUS_ERR_INVALID_SPEC,
    STATUS_ERR_INVALID_QUANTITY,
    STATUS_ERR_INVALID_NAME,
    STATUS_ERR_INVALID_INPUT,
    STATUS_ERR_INSUFFICIENT_STOCK,
    STATUS_ERR_CAPACITY_FULL,
    STATUS_ERR_NOT_FOUND,
    STATUS_ERR_BUFFER_TOO_SMALL,
    STATUS_ERR_FILE_OPEN,
    STATUS_ERR_FILE_WRITE,
    STATUS_ERR_FILE_FORMAT,
    STATUS_ERR_INPUT_CLOSED
} StatusCode;

/* How much of a component is left in the lab. */
typedef enum {
    STOCK_OUT = 0,
    STOCK_LOW,
    STOCK_OK
} StockLevel;

/* One line in the log: who holds how many of which item. */
typedef struct {
    char          borrower[NAME_SIZE];
    ComponentType component;
    char          spec[SPEC_SIZE];
    int           quantity;
} IssueRecord;

/* Everything the program remembers. */
typedef struct {
    int         available[COMP_COUNT];
    IssueRecord records[MAX_RECORDS];
    int         recordCount;
} Inventory;

#endif
