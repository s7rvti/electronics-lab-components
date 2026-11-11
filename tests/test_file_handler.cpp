#include <gtest/gtest.h>
#include <cstdio>

extern "C" {
#include "file_handler.h"
#include "inventory.h"
}

#define TEST_FILE "test_data_tmp.txt"

class FileHandlerTest : public ::testing::Test {
protected:
    Inventory inv;

    void SetUp() override {
        std::remove(TEST_FILE);
        initInventory(&inv);
    }
    void TearDown() override { std::remove(TEST_FILE); }

    /* Writes raw text into the temporary data file. */
    void writeFile(const char *text) {
        FILE *f = std::fopen(TEST_FILE, "w");
        ASSERT_NE(nullptr, f);
        std::fputs(text, f);
        std::fclose(f);
    }
};

TEST_F(FileHandlerTest, SaveThenLoadRestoresEverything) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    issueComponent(&inv, "Riya", COMP_TRANSISTOR, 0, 2);
    issueComponent(&inv, "Riya", COMP_JUMPER_WIRE, 0, 7);
    ASSERT_EQ(STATUS_OK, saveInventory(&inv, TEST_FILE));

    Inventory loaded;
    initInventory(&loaded);
    ASSERT_EQ(STATUS_OK, loadInventory(&loaded, TEST_FILE));

    for (int i = 0; i < COMP_COUNT; i++)
        EXPECT_EQ(inv.available[i], loaded.available[i]);
    ASSERT_EQ(3, loaded.recordCount);
    EXPECT_STREQ("Amit", loaded.records[0].borrower);
    EXPECT_STREQ("10k", loaded.records[0].spec);
    EXPECT_STREQ("BC547", loaded.records[1].spec);
    EXPECT_STREQ(NO_SPEC_LABEL, loaded.records[2].spec);
    EXPECT_EQ(7, loaded.records[2].quantity);
}

TEST_F(FileHandlerTest, FullStockWithNoRecordsRoundTrips) {
    ASSERT_EQ(STATUS_OK, saveInventory(&inv, TEST_FILE));
    Inventory loaded;
    ASSERT_EQ(STATUS_OK, loadInventory(&loaded, TEST_FILE));
    EXPECT_EQ(0, loaded.recordCount);
    EXPECT_EQ(getComponentTotal(COMP_RESISTOR), loaded.available[COMP_RESISTOR]);
}

TEST_F(FileHandlerTest, ReturnedItemsAreSavedToo) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    returnComponent(&inv, 0, 4);
    ASSERT_EQ(STATUS_OK, saveInventory(&inv, TEST_FILE));
    Inventory loaded;
    ASSERT_EQ(STATUS_OK, loadInventory(&loaded, TEST_FILE));
    EXPECT_EQ(0, loaded.recordCount);
}

TEST_F(FileHandlerTest, MissingFileReportsFileOpenError) {
    Inventory loaded;
    EXPECT_EQ(STATUS_ERR_FILE_OPEN, loadInventory(&loaded, TEST_FILE));
}

TEST_F(FileHandlerTest, EmptyFileIsRejected) {
    writeFile("");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, CountAboveTotalIsRejected) {
    writeFile("9999\n100\n30\n50\n30\n200\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, NegativeCountIsRejected) {
    writeFile("-1\n100\n30\n50\n30\n200\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, NonNumericCountIsRejected) {
    writeFile("abc\n100\n30\n50\n30\n200\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, RecordWithBadComponentNumberIsRejected) {
    writeFile("97\n100\n30\n50\n30\n200\nAmit 42 10k 3\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, RecordWithUnknownSpecIsRejected) {
    writeFile("97\n100\n30\n50\n30\n200\nAmit 0 BC547 3\n");   /* BC547 is not a resistor */
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, RecordWithTrailingJunkOrMissingFieldIsRejected) {
    writeFile("97\n100\n30\n50\n30\n200\nAmit 0 10k 3 extra\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
    writeFile("97\n100\n30\n50\n30\n200\nAmit 0 10k\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, CountsThatDoNotMatchRecordsAreRejected) {
    /* 3 resistors are issued but the available count says 100 (should be 97) */
    writeFile("100\n100\n30\n50\n30\n200\nAmit 0 10k 3\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
}

TEST_F(FileHandlerTest, FailedLoadLeavesCallersDataUntouched) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    writeFile("garbage\n");
    EXPECT_EQ(STATUS_ERR_FILE_FORMAT, loadInventory(&inv, TEST_FILE));
    EXPECT_EQ(1, inv.recordCount);
    EXPECT_EQ(getComponentTotal(COMP_RESISTOR) - 4, inv.available[COMP_RESISTOR]);
}

TEST_F(FileHandlerTest, BlankLinesBetweenRecordsAreIgnored) {
    writeFile("97\n100\n30\n50\n30\n200\n\nAmit 0 10k 3\n\n");
    ASSERT_EQ(STATUS_OK, loadInventory(&inv, TEST_FILE));
    EXPECT_EQ(1, inv.recordCount);
}

TEST_F(FileHandlerTest, NullArgumentsAreRejected) {
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, saveInventory(nullptr, TEST_FILE));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, saveInventory(&inv, nullptr));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, loadInventory(nullptr, TEST_FILE));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, loadInventory(&inv, nullptr));
}

TEST_F(FileHandlerTest, SavingToAnUnwritablePathReportsFileOpenError) {
    EXPECT_EQ(STATUS_ERR_FILE_OPEN, saveInventory(&inv, "no_such_folder/x/data.txt"));
}

TEST_F(FileHandlerTest, SampleDataFileLoadsCleanly) {
    ASSERT_EQ(STATUS_OK, loadInventory(&inv, "data/sample_data.txt"));
    EXPECT_EQ(8, inv.recordCount);
    EXPECT_EQ(0, inv.available[COMP_INDUCTOR]);
}
