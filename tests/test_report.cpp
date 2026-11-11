#include <gtest/gtest.h>
#include <cstdio>
#include <cstring>
#include <string>

extern "C" {
#include "report.h"
#include "inventory.h"
}

class ReportTest : public ::testing::Test {
protected:
    Inventory inv;
    void SetUp() override { initInventory(&inv); }

    /* Runs a report writer into a temporary buffer and returns the text. */
    template <typename Writer>
    std::string capture(Writer writer, StatusCode *status = nullptr) {
        FILE *f = std::tmpfile();
        EXPECT_NE(nullptr, f);
        StatusCode s = writer(f);
        if (status) *status = s;
        std::rewind(f);
        std::string text;
        char buffer[256];
        size_t n;
        while ((n = std::fread(buffer, 1, sizeof(buffer), f)) > 0)
            text.append(buffer, n);
        std::fclose(f);
        return text;
    }
};

TEST(StockLevelLabel, GivesReadableText) {
    EXPECT_STREQ("OUT OF STOCK", getStockLevelLabel(STOCK_OUT));
    EXPECT_STREQ("LOW", getStockLevelLabel(STOCK_LOW));
    EXPECT_STREQ("OK", getStockLevelLabel(STOCK_OK));
}

TEST_F(ReportTest, StockReportListsEveryComponentWhenFullyStocked) {
    StatusCode status;
    std::string text = capture([&](FILE *f) { return writeStockReport(&inv, f); }, &status);
    EXPECT_EQ(STATUS_OK, status);
    for (int i = 0; i < COMP_COUNT; i++)
        EXPECT_NE(std::string::npos, text.find(getComponentName(static_cast<ComponentType>(i))));
    EXPECT_NE(std::string::npos, text.find("All components are sufficiently stocked."));
    EXPECT_EQ(std::string::npos, text.find("LOW"));
}

TEST_F(ReportTest, StockReportFlagsLowAndOutOfStock) {
    issueComponent(&inv, "Amit", COMP_MOSFET, 0, 28);                       /* 2 left  -> LOW */
    issueComponent(&inv, "Amit", COMP_INDUCTOR, 0, getComponentTotal(COMP_INDUCTOR)); /* OUT */
    std::string text = capture([&](FILE *f) { return writeStockReport(&inv, f); });
    EXPECT_NE(std::string::npos, text.find("LOW"));
    EXPECT_NE(std::string::npos, text.find("OUT OF STOCK"));
    EXPECT_NE(std::string::npos, text.find("Restock needed for 2 component type(s)."));
}

TEST_F(ReportTest, StockReportShowsScarcestComponentFirst) {
    issueComponent(&inv, "Amit", COMP_MOSFET, 0, 28);
    std::string text = capture([&](FILE *f) { return writeStockReport(&inv, f); });
    EXPECT_LT(text.find("MOSFET"), text.find("Resistor"));
}

TEST_F(ReportTest, StockReportShowsIssuedItemTotal) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 0, 4);
    issueComponent(&inv, "Riya", COMP_JUMPER_WIRE, 0, 6);
    std::string text = capture([&](FILE *f) { return writeStockReport(&inv, f); });
    EXPECT_NE(std::string::npos, text.find("Items currently issued: 10"));
}

TEST_F(ReportTest, IssuedReportSaysWhenNothingIsIssued) {
    std::string text = capture([&](FILE *f) { return writeIssuedReport(&inv, f); });
    EXPECT_NE(std::string::npos, text.find("Nothing is currently issued."));
}

TEST_F(ReportTest, IssuedReportListsBorrowersAndSpecs) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    issueComponent(&inv, "Riya", COMP_TRANSISTOR, 0, 2);
    std::string text = capture([&](FILE *f) { return writeIssuedReport(&inv, f); });
    EXPECT_NE(std::string::npos, text.find("Amit"));
    EXPECT_NE(std::string::npos, text.find("10k"));
    EXPECT_NE(std::string::npos, text.find("Riya"));
    EXPECT_NE(std::string::npos, text.find("BC547"));
}

TEST_F(ReportTest, BorrowerReportShowsOnlyThatPerson) {
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    issueComponent(&inv, "Riya", COMP_TRANSISTOR, 0, 2);
    std::string text = capture([&](FILE *f) { return writeBorrowerReport(&inv, "Amit", f); });
    EXPECT_NE(std::string::npos, text.find("10k"));
    EXPECT_EQ(std::string::npos, text.find("BC547"));
}

TEST_F(ReportTest, BorrowerReportForUnknownPersonSaysHoldsNothing) {
    std::string text = capture([&](FILE *f) { return writeBorrowerReport(&inv, "Nobody", f); });
    EXPECT_NE(std::string::npos, text.find("Nobody currently holds nothing."));
}

TEST_F(ReportTest, WritersRejectNullArguments) {
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, writeStockReport(nullptr, stdout));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, writeStockReport(&inv, nullptr));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, writeIssuedReport(&inv, nullptr));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, writeBorrowerReport(&inv, nullptr, stdout));
    EXPECT_EQ(STATUS_ERR_NULL_POINTER, saveReportToFile(&inv, nullptr));
}

TEST_F(ReportTest, SaveReportToFileWritesAndCanBeReadBack) {
    const char *path = "test_report_tmp.txt";
    issueComponent(&inv, "Amit", COMP_RESISTOR, 5, 4);
    ASSERT_EQ(STATUS_OK, saveReportToFile(&inv, path));
    FILE *f = std::fopen(path, "r");
    ASSERT_NE(nullptr, f);
    char buffer[2048] = {0};
    size_t n = std::fread(buffer, 1, sizeof(buffer) - 1, f);
    std::fclose(f);
    std::remove(path);
    EXPECT_GT(n, 0u);
    EXPECT_NE(nullptr, std::strstr(buffer, "STOCK REPORT"));
    EXPECT_NE(nullptr, std::strstr(buffer, "ISSUED COMPONENTS"));
}

TEST_F(ReportTest, SaveReportToUnwritablePathReportsFileOpenError) {
    EXPECT_EQ(STATUS_ERR_FILE_OPEN, saveReportToFile(&inv, "no_such_folder/x/report.txt"));
}
