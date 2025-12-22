CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c99
CPPFLAGS ?= -I$(SRC_DIR) -D_POSIX_C_SOURCE=200809L

SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TEST_DIR := tests

SRCS := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/command.c \
	$(SRC_DIR)/config.c \
	$(SRC_DIR)/engine.c \
	$(SRC_DIR)/parser.c \
	$(SRC_DIR)/date_util.c

OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
BIN := $(BIN_DIR)/astro

TEST_BINS := \
	$(TEST_DIR)/bin/test_config \
	$(TEST_DIR)/bin/test_date_util \
	$(TEST_DIR)/bin/test_engine \
	$(TEST_DIR)/bin/test_parser

all: $(BIN)

$(BIN): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR) $(TEST_DIR)/bin:
	mkdir -p $@

tests: $(TEST_BINS)

$(TEST_DIR)/bin/test_config: $(TEST_DIR)/test_config.c $(SRC_DIR)/config.c | $(TEST_DIR)/bin
	$(CC) $(CPPFLAGS) -I$(TEST_DIR) $(CFLAGS) $^ -o $@

$(TEST_DIR)/bin/test_date_util: $(TEST_DIR)/test_date_util.c $(SRC_DIR)/date_util.c | $(TEST_DIR)/bin
	$(CC) $(CPPFLAGS) -I$(TEST_DIR) $(CFLAGS) $^ -o $@

$(TEST_DIR)/bin/test_engine: $(TEST_DIR)/test_engine.c $(SRC_DIR)/engine.c | $(TEST_DIR)/bin
	$(CC) $(CPPFLAGS) -I$(TEST_DIR) $(CFLAGS) $^ -o $@

$(TEST_DIR)/bin/test_parser: $(TEST_DIR)/test_parser.c $(SRC_DIR)/parser.c | $(TEST_DIR)/bin
	$(CC) $(CPPFLAGS) -I$(TEST_DIR) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(TEST_DIR)/bin

.PHONY: all clean tests
