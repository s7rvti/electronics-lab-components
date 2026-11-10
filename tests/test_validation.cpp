#include <gtest/gtest.h>
#include <string>

extern "C" {
#include "validation.h"
}

/* ---------------- isValidName ---------------- */

TEST(IsValidName, AcceptsLettersDigitsUnderscoreAndDash) {
    EXPECT_EQ(1, isValidName("Amit"));
    EXPECT_EQ(1, isValidName("student_01"));
    EXPECT_EQ(1, isValidName("A-B"));
}

TEST(IsValidName, RejectsEmptyAndNull) {
    EXPECT_EQ(0, isValidName(""));
    EXPECT_EQ(0, isValidName(nullptr));
}

TEST(IsValidName, RejectsSpacesAndSymbols) {
    EXPECT_EQ(0, isValidName("Amit Kumar"));
    EXPECT_EQ(0, isValidName("am!t"));
    EXPECT_EQ(0, isValidName("a/b"));
}

TEST(IsValidName, BoundaryLengthOneAndMax) {
    EXPECT_EQ(1, isValidName("A"));
    EXPECT_EQ(1, isValidName(std::string(NAME_MAX_CHARS, 'x').c_str()));
    EXPECT_EQ(0, isValidName(std::string(NAME_MAX_CHARS + 1, 'x').c_str()));
}

/* ---------------- parseInteger ---------------- */

TEST(ParseInteger, ParsesPlainAndSignedNumbers) {
    int value = 0;
    EXPECT_EQ(STATUS_OK, parseInteger("42", &value));
    EXPECT_EQ(42, value);
    EXPECT_EQ(STATUS_OK, parseInteger("-7", &value));
    EXPECT_EQ(-7, value);
}

TEST(ParseInteger, AllowsSurroundingSpaces) {
    int value = 0;
    EXPECT_EQ(STATUS_OK, parseInteger("  15  ", &value));
    EXPECT_EQ(15, value);
}

TEST(ParseInteger, RejectsEmptyLettersDecimalsAndTrailingJunk) {
    int value = 99;
    EXPECT_EQ(STATUS_ERR_INVALID_INPUT, parseInteger("", &value));
    EXPECT_EQ(STATUS_ERR_INVALID_INPUT, parseInteger("abc", &value));
    EXPECT_EQ(STATUS_ERR_INVALID_INPUT, parseInteger("5.5", &value));
    EXPECT_EQ(STATUS_ERR_INVALID_INPUT, parseInteger("5 apples", &value));
    EXPECT_EQ(99, value);                       /* untouched on failure */
}

TEST(ParseInteger, BoundaryOfAllowedTextLength) {
    int value = 0;
    EXPECT_EQ(STATUS_OK, parseInteger("999999999", &value));            /* 9 chars  */
    EXPECT_EQ(999999999, value);
    EXPECT_EQ(STATUS_ERR_INVALID_INPUT, parseInteger("9999999999", &value));  /* 10 chars */
}

TEST(ParseInteger, RejectsNullPointers) {
    int value = 0;
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, parseInteger(nullptr, &value));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, parseInteger("5", nullptr));
}

/* ---------------- isInRange / isValidComponent ---------------- */

TEST(IsInRange, BoundariesAreInclusive) {
    EXPECT_EQ(1, isInRange(1, 1, 5));
    EXPECT_EQ(1, isInRange(5, 1, 5));
    EXPECT_EQ(0, isInRange(0, 1, 5));
    EXPECT_EQ(0, isInRange(6, 1, 5));
}

TEST(IsValidComponent, AcceptsFirstAndLastRejectsOutside) {
    EXPECT_EQ(1, isValidComponent(0));
    EXPECT_EQ(1, isValidComponent(COMP_COUNT - 1));
    EXPECT_EQ(0, isValidComponent(-1));
    EXPECT_EQ(0, isValidComponent(COMP_COUNT));
}

/* ---------------- string helpers ---------------- */

TEST(StringsEqual, ComparesWholeStrings) {
    EXPECT_EQ(1, stringsEqual("10k", "10k"));
    EXPECT_EQ(0, stringsEqual("10k", "10K"));
    EXPECT_EQ(0, stringsEqual("BC547", "BC54"));
    EXPECT_EQ(0, stringsEqual(nullptr, "x"));
}

TEST(CopyString, CopiesWhenItFits) {
    char buffer[6];
    EXPECT_EQ(STATUS_OK, copyString(buffer, sizeof(buffer), "BC547"));
    EXPECT_STREQ("BC547", buffer);
}

TEST(CopyString, RejectsWhenTooLongOrNull) {
    char buffer[5];
    EXPECT_EQ(STATUS_ERR_BUFFER_TOO_SMALL, copyString(buffer, sizeof(buffer), "BC547"));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, copyString(nullptr, 5, "x"));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, copyString(buffer, 5, nullptr));
}

TEST(TrimNewline, RemovesOnlyOneTrailingNewline) {
    char withNewline[] = "abc\n";
    char without[] = "abc";
    trimNewline(withNewline);
    trimNewline(without);
    EXPECT_STREQ("abc", withNewline);
    EXPECT_STREQ("abc", without);
}
