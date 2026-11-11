# Lab Component Tracker - build automation
#   make all    build the program
#   make run    build and run the program
#   make test   build and run the Google Test unit tests
#   make clean  remove everything that was generated

CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -Wpedantic -std=c99 -Iinclude
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++17 -Iinclude

# If gtest is not in the default compiler paths, override these, e.g.
#   make test GTEST_INC="-isystem /path/include" GTEST_LIB="-L/path/lib"
GTEST_INC  =
GTEST_LIB  =
GTEST_LIBS = -lgtest -lgtest_main -pthread

SRC_DIR   = src
TEST_DIR  = tests
BUILD_DIR = build
APP       = lab_tracker
TEST_APP  = $(BUILD_DIR)/run_tests

# Modules shared by the program and the tests (everything except main.c)
MODULES   = inventory validation file_handler report ui
MODULE_OBJS = $(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(MODULES)))
MAIN_OBJ    = $(BUILD_DIR)/main.o

TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/test_%.o,$(TEST_SRCS))

DEPS = $(MODULE_OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJS:.o=.d)

all: $(APP)

$(APP): $(MAIN_OBJ) $(MODULE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(APP)
	./$(APP)

test: $(TEST_APP)
	./$(TEST_APP)

$(TEST_APP): $(TEST_OBJS) $(MODULE_OBJS)
	$(CXX) -o $@ $^ $(GTEST_LIB) $(GTEST_LIBS)

# Compile each source file separately; -MMD writes header dependencies
# so only files affected by a changed header are rebuilt.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(GTEST_INC) -MMD -MP -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(APP) test_data_tmp.txt test_report_tmp.txt

-include $(DEPS)

.PHONY: all run test clean
