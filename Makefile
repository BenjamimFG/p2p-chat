CC := gcc
CFLAGS := -std=c17 -Wall -Wextra

LIBS := ncurses pthread
LIB_FLAGS := $(addprefix -l,$(LIBS))

SRC_DIR := src
BUILD_DIR := build

SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIR := include
INC_DIRS := $(shell find $(INC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

TARGET := p2p_chat

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LIB_FLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
