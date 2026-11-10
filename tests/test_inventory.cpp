#include <gtest/gtest.h>
#include <cstdio>
#include <string>

extern "C" {
#include "inventory.h"
}

/* Every test starts with a fresh, full inventory. */
class InventoryTest : public ::testing::Test {
protected:
    Inventory inv;
    void SetUp() override { initInventory(&inv); }

    int total(ComponentType c) { return getComponentTotal(c); }
};

/* ---------------- catalogue ---------------- */

TEST_F(InventoryTest, InitGivesFullStockAndNoRecords) {
    for (int i = 0; i < COMP_COUNT; i++)
        EXPECT_EQ(total(static_cast<ComponentType>(i)), inv.available[i]);
    EXPECT_EQ(0, inv.recordCount);
}

TEST_F(InventoryTest, CatalogueLookupsHandleInvalidTypes) {
    EXPECT_STREQ("Resistor", getComponentName(COMP_RESISTOR));
    EXPECT_STREQ("Unknown", getComponentName(static_cast<ComponentType>(-1)));
    EXPECT_EQ(0, getComponentTotal(static_cast<ComponentType>(COMP_COUNT)));
}

TEST_F(InventoryTest, SpecLookupBoundaries) {
    EXPECT_EQ(9, getSpecCount(COMP_RESISTOR));
    EXPECT_STREQ("100", getSpecName(COMP_RESISTOR, 0));
    EXPECT_STREQ("1M", getSpecName(COMP_RESISTOR, 8));
    EXPECT_EQ(nullptr, getSpecName(COMP_RESISTOR, 9));
    EXPECT_EQ(nullptr, getSpecName(COMP_RESISTOR, -1));
    EXPECT_EQ(0, getSpecCount(COMP_JUMPER_WIRE));
}

TEST_F(InventoryTest, IsKnownSpecChecksPerComponent) {
    EXPECT_EQ(1, isKnownSpec(COMP_TRANSISTOR, "BC547"));
    EXPECT_EQ(0, isKnownSpec(COMP_RESISTOR, "BC547"));     /* wrong component */
    EXPECT_EQ(1, isKnownSpec(COMP_JUMPER_WIRE, NO_SPEC_LABEL));
    EXPECT_EQ(0, isKnownSpec(COMP_JUMPER_WIRE, "10k"));
}

/* ---------------- issuing ---------------- */

TEST_F(InventoryTest, IssueReducesStockAndCreatesRecord) {
    EXPECT_EQ(STATUS_OK, issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4));   /* 10k */
    EXPECT_EQ(total(COMP_RESISTOR) - 4, inv.available[COMP_RESISTOR]);
    ASSERT_EQ(1, inv.recordCount);
    EXPECT_STREQ("Amit", inv.records[0].borrower);
    EXPECT_STREQ("10k", inv.records[0].spec);
    EXPECT_EQ(COMP_RESISTOR, inv.records[0].component);
    EXPECT_EQ(4, inv.records[0].quantity);
}

TEST_F(InventoryTest, IssueJumperWiresIgnoresSpecIndex) {
    EXPECT_EQ(STATUS_OK, issueComponent(&inv, "Amit", COMP_JUMPER_WIRE, 99, 10));
    EXPECT_STREQ(NO_SPEC_LABEL, inv.records[0].spec);
}

TEST_F(InventoryTest, IssueQuantityBoundaries) {
    int all = total(COMP_MOSFET);
    EXPECT_EQ(STATUS_ERR_INVALID_QUANTITY, issueComponent(&inv, "Amit", COMP_MOSFET, 0, 0));
    EXPECT_EQ(STATUS_ERR_INVALID_QUANTITY, issueComponent(&inv, "Amit", COMP_MOSFET, 0, -1));
    EXPECT_EQ(STATUS_ERR_INSUFFICIENT_STOCK, issueComponent(&inv, "Amit", COMP_MOSFET, 0, all + 1));
    EXPECT_EQ(all, inv.available[COMP_MOSFET]);              /* nothing changed */
    EXPECT_EQ(STATUS_OK, issueComponent(&inv, "Amit", COMP_MOSFET, 0, all));   /* exactly all */
    EXPECT_EQ(0, inv.available[COMP_MOSFET]);
    EXPECT_EQ(STATUS_ERR_INSUFFICIENT_STOCK, issueComponent(&inv, "Riya", COMP_MOSFET, 0, 1));
}

TEST_F(InventoryTest, IssueRejectsBadComponentSpecAndName) {
    EXPECT_EQ(STATUS_ERR_INVALID_COMPONENT,
              issueComponent(&inv, "Amit", static_cast<ComponentType>(COMP_COUNT), 0, 1));
    EXPECT_EQ(STATUS_ERR_INVALID_SPEC, issueComponent(&inv, "Amit", COMP_RESISTOR, -1, 1));
    EXPECT_EQ(STATUS_ERR_INVALID_SPEC,
              issueComponent(&inv, "Amit", COMP_RESISTOR, getSpecCount(COMP_RESISTOR), 1));
    EXPECT_EQ(STATUS_ERR_INVALID_NAME, issueComponent(&inv, "", COMP_RESISTOR, 0, 1));
    EXPECT_EQ(STATUS_ERR_INVALID_NAME, issueComponent(&inv, "Amit Kumar", COMP_RESISTOR, 0, 1));
    EXPECT_EQ(0, inv.recordCount);
}

TEST_F(InventoryTest, IssueRejectsNullPointers) {
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, issueComponent(nullptr, "Amit", COMP_RESISTOR, 0, 1));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, issueComponent(&inv, nullptr, COMP_RESISTOR, 0, 1));
}

TEST_F(InventoryTest, SameItemTwiceMergesIntoOneRecord) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 2);
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 3);
    EXPECT_EQ(1, inv.recordCount);
    EXPECT_EQ(5, inv.records[0].quantity);
}

TEST_F(InventoryTest, DifferentSpecOrPersonMakesNewRecord) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 2);
    issueComponent(&inv, "Amit", COMP_RESISTOR, 3, 2);   /* other spec   */
    issueComponent(&inv, "Riya", COMP_RESISTOR, 5, 2);   /* other person */
    EXPECT_EQ(3, inv.recordCount);
}

TEST_F(InventoryTest, IssueFailsWhenRecordListIsFull) {
    char name[NAME_SIZE];
    for (int i = 0; i < MAX_RECORDS; i++) {
        std::snprintf(name, sizeof(name), "user%d", i);
        ASSERT_EQ(STATUS_OK, issueComponent(&inv, name, COMP_RESISTOR, 0, 1));
    }
    EXPECT_EQ(STATUS_ERR_CAPACITY_FULL, issueComponent(&inv, "extra", COMP_CAPACITOR, 0, 1));
    EXPECT_EQ(total(COMP_CAPACITOR), inv.available[COMP_CAPACITOR]);
}

/* ---------------- returning ---------------- */

TEST_F(InventoryTest, ReturnAllRestoresStockAndRemovesRecord) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    EXPECT_EQ(STATUS_OK, returnComponent(&inv, 0, 4));
    EXPECT_EQ(total(COMP_RESISTOR), inv.available[COMP_RESISTOR]);
    EXPECT_EQ(0, inv.recordCount);
}

TEST_F(InventoryTest, PartialReturnKeepsRecord) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    EXPECT_EQ(STATUS_OK, returnComponent(&inv, 0, 1));
    EXPECT_EQ(3, inv.records[0].quantity);
    EXPECT_EQ(total(COMP_RESISTOR) - 3, inv.available[COMP_RESISTOR]);
}

TEST_F(InventoryTest, ReturnQuantityBoundaries) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 2);
    EXPECT_EQ(STATUS_ERR_INVALID_QUANTITY, returnComponent(&inv, 0, 0));
    EXPECT_EQ(STATUS_ERR_INVALID_QUANTITY, returnComponent(&inv, 0, -1));
    EXPECT_EQ(STATUS_ERR_INVALID_QUANTITY, returnComponent(&inv, 0, 3));   /* more than held */
    EXPECT_EQ(2, inv.records[0].quantity);
}

TEST_F(InventoryTest, ReturnBadRecordIndexFails) {
    EXPECT_EQ(STATUS_ERR_NOT_FOUND, returnComponent(&inv, 0, 1));          /* nothing issued */
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 2);
    EXPECT_EQ(STATUS_ERR_NOT_FOUND, returnComponent(&inv, -1, 1));
    EXPECT_EQ(STATUS_ERR_NOT_FOUND, returnComponent(&inv, 1, 1));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, returnComponent(nullptr, 0, 1));
}

TEST_F(InventoryTest, RemovingMiddleRecordKeepsTheOthersInOrder) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 1);
    issueComponent(&inv, "Riya", COMP_CAPACITOR, 1, 2);
    issueComponent(&inv, "Zoya", COMP_TRANSISTOR, 0, 3);
    EXPECT_EQ(STATUS_OK, returnComponent(&inv, 1, 2));
    ASSERT_EQ(2, inv.recordCount);
    EXPECT_STREQ("Amit", inv.records[0].borrower);
    EXPECT_STREQ("Zoya", inv.records[1].borrower);
    EXPECT_STREQ("BC547", inv.records[1].spec);
    EXPECT_EQ(3, inv.records[1].quantity);
}

/* ---------------- searching ---------------- */

TEST_F(InventoryTest, FindHeldRecordsMatchesPersonAndComponent) {
    int matches[MAX_RECORDS];
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 2);
    issueComponent(&inv, "Amit", COMP_RESISTOR, 3, 1);
    issueComponent(&inv, "Amit", COMP_CAPACITOR, 0, 1);
    EXPECT_EQ(2, findHeldRecords(&inv, "Amit", COMP_RESISTOR, matches, MAX_RECORDS));
    EXPECT_EQ(0, matches[0]);
    EXPECT_EQ(1, matches[1]);
    EXPECT_EQ(0, findHeldRecords(&inv, "Amit", COMP_INDUCTOR, matches, MAX_RECORDS));
    EXPECT_EQ(0, findHeldRecords(&inv, "Riya", COMP_RESISTOR, matches, MAX_RECORDS));
}

TEST_F(InventoryTest, FindHeldRecordsRespectsMaxMatchesAndNulls) {
    int matches[1];
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 2);
    issueComponent(&inv, "Amit", COMP_RESISTOR, 3, 1);
    EXPECT_EQ(1, findHeldRecords(&inv, "Amit", COMP_RESISTOR, matches, 1));
    EXPECT_EQ(0, findHeldRecords(nullptr, "Amit", COMP_RESISTOR, matches, 1));
}

TEST_F(InventoryTest, FindRecordsByBorrowerSpansComponents) {
    int matches[MAX_RECORDS];
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 2);
    issueComponent(&inv, "Riya", COMP_CAPACITOR, 0, 1);
    issueComponent(&inv, "Amit", COMP_MOSFET, 0, 1);
    EXPECT_EQ(2, findRecordsByBorrower(&inv, "Amit", matches, MAX_RECORDS));
    EXPECT_EQ(0, findRecordsByBorrower(&inv, "Nobody", matches, MAX_RECORDS));
}

/* ---------------- calculations and status ---------------- */

TEST_F(InventoryTest, UtilisationAndAvailablePercent) {
    EXPECT_EQ(0, calculateUtilisationPercent(&inv, COMP_RESISTOR));
    EXPECT_EQ(100, calculateAvailablePercent(&inv, COMP_RESISTOR));
    issueComponent(&inv, "Amit", COMP_RESISTOR, 0, 25);            /* 25 of 100 */
    EXPECT_EQ(25, calculateUtilisationPercent(&inv, COMP_RESISTOR));
    EXPECT_EQ(75, calculateAvailablePercent(&inv, COMP_RESISTOR));
    issueComponent(&inv, "Riya", COMP_RESISTOR, 0, 75);            /* everything */
    EXPECT_EQ(100, calculateUtilisationPercent(&inv, COMP_RESISTOR));
}

TEST_F(InventoryTest, CountIssuedItemsSumsAllRecords) {
    EXPECT_EQ(0, countIssuedItems(&inv));
    issueComponent(&inv, "Amit", COMP_RESISTOR, 0, 4);
    issueComponent(&inv, "Riya", COMP_JUMPER_WIRE, 0, 10);
    EXPECT_EQ(14, countIssuedItems(&inv));
    EXPECT_EQ(0, countIssuedItems(nullptr));
}

TEST_F(InventoryTest, StockLevelBoundaryAtTwentyPercent) {
    /* Resistor total is 100: LOW at 20 left, OK at 21 left. */
    issueComponent(&inv, "Amit", COMP_RESISTOR, 0, 79);            /* 21 left */
    EXPECT_EQ(STOCK_OK, getStockLevel(&inv, COMP_RESISTOR));
    issueComponent(&inv, "Amit", COMP_RESISTOR, 0, 1);             /* 20 left */
    EXPECT_EQ(STOCK_LOW, getStockLevel(&inv, COMP_RESISTOR));
    issueComponent(&inv, "Amit", COMP_RESISTOR, 0, 19);            /* 1 left  */
    EXPECT_EQ(STOCK_LOW, getStockLevel(&inv, COMP_RESISTOR));
    issueComponent(&inv, "Amit", COMP_RESISTOR, 0, 1);             /* 0 left  */
    EXPECT_EQ(STOCK_OUT, getStockLevel(&inv, COMP_RESISTOR));
}

TEST_F(InventoryTest, SortPutsScarcestComponentFirst) {
    ComponentType order[COMP_COUNT];
    issueComponent(&inv, "Amit", COMP_MOSFET, 0, 28);              /* 2 of 30 left  */
    issueComponent(&inv, "Amit", COMP_CAPACITOR, 0, 50);           /* 50 of 100 left */
    ASSERT_EQ(STATUS_OK, sortComponentsByScarcity(&inv, order));
    EXPECT_EQ(COMP_MOSFET, order[0]);
    EXPECT_EQ(COMP_CAPACITOR, order[1]);
    /* the rest are full (100%) and keep catalogue order */
    EXPECT_EQ(COMP_RESISTOR, order[2]);
}

TEST_F(InventoryTest, SortRejectsNullPointers) {
    ComponentType order[COMP_COUNT];
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, sortComponentsByScarcity(nullptr, order));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, sortComponentsByScarcity(&inv, nullptr));
}
