TARGET = msh
PREFIX ?= /usr/local
VERSION = 0.1.0

CC ?= gcc
BUILD ?= debug

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
OBJ_DIR = build
BIN_DIR = build/bin

CORE_SRCS = $(wildcard $(SRC_DIR)/core/*.c)
UTILS_SRCS = $(wildcard $(SRC_DIR)/utils/*.c)
MAIN_SRC = $(SRC_DIR)/main.c

SRCS = $(CORE_SRCS) $(UTILS_SRCS) $(MAIN_SRC)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

INC_PATHS = -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/core -I$(INCLUDE_DIR)/utils

PKG_CONFIG ?= pkg-config

XML2_CFLAGS := $(shell $(PKG_CONFIG) --cflags libxml-2.0)
XML2_LIBS   := $(shell $(PKG_CONFIG) --libs  libxml-2.0)

CFLAGS ?= -ansi -std=c17
CFLAGS += $(XML2_CFLAGS)

CFLAGS += $(shell ${PKG_CONFIG} --cflags readline)
LDFLAGS := $(shell ${PKG_CONFIG} --libs readline)
LDFLAGS += $(XML2_LIBS)

ifeq ($(BUILD),release)
    CFLAGS += -O2 -DNDEBUG
else
    CFLAGS += -g -O0 -DDEBUG
endif

.PHONY: all clean install uninstall

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(INC_PATHS)
	# gcc build/main.o -o build/bin/msh -lreadline

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(INC_PATHS)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(INC_PATHS) -MM -MT $(@:.d=.o) $< > $@

-include $(DEPS)

$(BIN_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

install: release
	install -d $(PREFIX)/bin
	install -m 755 $(BIN_DIR)/$(TARGET) $(PREFIX)/bin/

uninstall:
	@rm -f $(PREFIX)/bin/$(TARGET)

clean:
	@rm -rf $(OBJ_DIR)
	# rm -f $(TEST_OBJS) $(TEST_TARGETS)

help:
	@echo "Unix Shell Emulator Build System"
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  debug     - Build with debug symbols"
	@echo "  release   - Build with optimizations"
	@echo "  test      - Run tests"
	@echo "  docs      - Generate documentation"
	@echo "  install   - Install to system"
	@echo "  uninstall - Remove from system"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help"

.DEFAULT_GOAL := all