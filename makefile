# Compiler and Tools
CC := clang
PKG_CONFIG := pkg-config

# Directories
SRC_DIR := src
TEST_DIR := test
BUILD_DIR := build
DEPS_DIR := $(BUILD_DIR)/deps
UNITY_DIR := test-framework

# Binary output
BIN := chip8-emulator

# SDL2 flags via pkg-config
SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl2 2>/dev/null)
SDL_LIBS := $(shell $(PKG_CONFIG) --libs sdl2 2>/dev/null)

# Compiler flags
WARNFLAGS := -Wall -Wextra -Wpedantic -Wshadow -Wformat=2
CFLAGS := $(WARNFLAGS) -std=c11 $(SDL_CFLAGS)
OPTFLAGS := -O2
DEBUGFLAGS := -g3 -O0 -DDEBUG
ASANFLAGS := -fsanitize=address -fno-common -fno-omit-frame-pointer

# Dependency generation flags
DEPFLAGS = -MMD -MP -MF $(DEPS_DIR)/$*.d

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Test files (exclude main.o from test builds)
TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/test_%.o,$(TEST_SRCS))
SRC_OBJS_NO_MAIN := $(filter-out $(BUILD_DIR)/main.o,$(OBJS))

# Unity test framework
UNITY_SRC := $(UNITY_DIR)/unity.c
UNITY_OBJ := $(BUILD_DIR)/unity.o

# Include paths for tests
TEST_INCLUDES := -I$(SRC_DIR) -I$(UNITY_DIR)

# Dependency files
DEPS := $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# Phony targets
.PHONY: all clean test memcheck debug release dirs help

# Default target
all: release

# Create build directories
dirs:
	@mkdir -p $(BUILD_DIR) $(DEPS_DIR)

# Release build
release: CFLAGS += $(OPTFLAGS)
release: dirs $(BIN)

# Debug build
debug: CFLAGS += $(DEBUGFLAGS)
debug: dirs $(BIN)

# Link the main binary
$(BIN): $(OBJS)
	@echo "Linking $@"
	@$(CC) $(CFLAGS) $^ $(SDL_LIBS) -o $@

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	@echo "Compiling $<"
	@mkdir -p $(DEPS_DIR)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Compile test files
$(BUILD_DIR)/test_%.o: $(TEST_DIR)/%.c | dirs
	@echo "Compiling test $<"
	@mkdir -p $(DEPS_DIR)
	@$(CC) $(CFLAGS) $(TEST_INCLUDES) $(DEPFLAGS) -c $< -o $@

# Compile Unity framework
$(UNITY_OBJ): $(UNITY_SRC) | dirs
	@echo "Compiling Unity framework"
	@$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: CFLAGS += $(DEBUGFLAGS)
test: dirs $(BUILD_DIR)/tests.out
	@echo "Running tests..."
	@./$(BUILD_DIR)/tests.out

$(BUILD_DIR)/tests.out: $(SRC_OBJS_NO_MAIN) $(TEST_OBJS) $(UNITY_OBJ)
	@echo "Linking tests"
	@$(CC) $(CFLAGS) $(TEST_INCLUDES) $^ -o $@

# Memory check with AddressSanitizer
memcheck: CFLAGS += $(DEBUGFLAGS) $(ASANFLAGS)
memcheck: dirs $(BUILD_DIR)/memcheck.out
	@echo "Running memory check..."
	@./$(BUILD_DIR)/memcheck.out
	@echo "Memory check passed!"

$(BUILD_DIR)/memcheck.out: $(SRC_OBJS_NO_MAIN) $(TEST_OBJS) $(UNITY_OBJ)
	@echo "Linking memcheck binary"
	@$(CC) $(CFLAGS) $(ASANFLAGS) $(TEST_INCLUDES) $^ -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(BIN)

# Help target
help:
	@echo "CHIP-8 Emulator Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build release version (default)"
	@echo "  release  - Build with optimizations (-O2)"
	@echo "  debug    - Build with debug symbols (-g3 -O0)"
	@echo "  test     - Build and run unit tests"
	@echo "  memcheck - Run tests with AddressSanitizer"
	@echo "  clean    - Remove all build artifacts"
	@echo "  help     - Show this help message"

# Include dependency files
-include $(wildcard $(DEPS_DIR)/*.d)
