#include <stdio.h>
#include "validation.h"

int stringLength(const char *text)
{
    int length = 0;

    if (text == NULL) {
        return 0;
    }
    while (text[length] != '\0') {
        length++;
    }
    return length;
}

int stringsEqual(const char *first, const char *second)
{
    int i = 0;

    if (first == NULL || second == NULL) {
        return 0;
    }
    while (first[i] != '\0' && second[i] != '\0') {
        if (first[i] != second[i]) {
            return 0;
        }
        i++;
    }
    return first[i] == second[i];
}

StatusCode copyString(char *dest, int destSize, const char *src)
{
    int i;

    if (dest == NULL || src == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    if (stringLength(src) >= destSize) {
        return STATUS_ERR_BUFFER_TOO_SMALL;
    }
    for (i = 0; src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return STATUS_OK;
}

void trimNewline(char *line)
{
    int length = stringLength(line);

    if (length > 0 && line[length - 1] == '\n') {
        line[length - 1] = '\0';
    }
}

int isInRange(int value, int min, int max)
{
    return value >= min && value <= max;
}

int isValidComponent(int component)
{
    return isInRange(component, 0, COMP_COUNT - 1);
}

/* Returns 1 for a letter, digit, '_' or '-'. */
static int isNameCharacter(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_' || c == '-';
}

int isValidName(const char *name)
{
    int i;
    int length = stringLength(name);

    if (!isInRange(length, 1, NAME_MAX_CHARS)) {
        return 0;
    }
    for (i = 0; i < length; i++) {
        if (!isNameCharacter(name[i])) {
            return 0;
        }
    }
    return 1;
}

StatusCode parseInteger(const char *text, int *value)
{
    int parsed;
    char extra;

    if (text == NULL || value == NULL) {
        return STATUS_ERR_NULL_POINTER;
    }
    if (stringLength(text) > MAX_INTEGER_TEXT_LEN) {
        return STATUS_ERR_INVALID_INPUT;
    }
    /* A second conversion succeeding means there was trailing junk. */
    if (sscanf(text, "%d %c", &parsed, &extra) != 1) {
        return STATUS_ERR_INVALID_INPUT;
    }
    *value = parsed;
    return STATUS_OK;
}
