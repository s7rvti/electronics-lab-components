/*
 * validation.h
 * Input checks and small text helpers (the project uses only stdio.h,
 * so the string helpers are written here instead of using string.h).
 */
#ifndef VALIDATION_H
#define VALIDATION_H

#include "lab_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_INTEGER_TEXT_LEN 9      /* longer numbers could overflow an int */

/* Returns the length of text, or 0 if text is NULL. */
int stringLength(const char *text);

/* Returns 1 if both strings are non-NULL and identical, otherwise 0. */
int stringsEqual(const char *first, const char *second);

/* Copies src into dest (size destSize, including '\0').
   Returns STATUS_OK, STATUS_ERR_NULL_POINTER or STATUS_ERR_BUFFER_TOO_SMALL. */
StatusCode copyString(char *dest, int destSize, const char *src);

/* Removes one trailing '\n' from line, if present. */
void trimNewline(char *line);

/* Returns 1 if min <= value <= max. */
int isInRange(int value, int min, int max);

/* Returns 1 if the number is a real ComponentType value. */
int isValidComponent(int component);

/* Returns 1 if name is 1..NAME_MAX_CHARS characters of letters, digits,
   '_' or '-' (no spaces, so the saved file stays easy to read back). */
int isValidName(const char *name);

/* Converts text such as "42" into an int. Rejects empty text, letters,
   decimals, trailing junk and numbers longer than MAX_INTEGER_TEXT_LEN.
   Returns STATUS_OK, STATUS_ERR_NULL_POINTER or STATUS_ERR_INVALID_INPUT. */
StatusCode parseInteger(const char *text, int *value);

#ifdef __cplusplus
}
#endif

#endif
