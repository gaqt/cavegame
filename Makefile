TARGET := out
BUILD_DIR := dist
RELEASE_DIR := $(BUILD_DIR)/release
DEBUG_DIR := $(BUILD_DIR)/debug
SRC_DIR := src
LIB_DIR := lib

LIBS := -lGL -lm -lpthread -ldl -lrt -lX11
LOCAL_LIB_NAMES := libraylib.a
CC := gcc
CXX := g++
COMMONFLAGS := -Wall -Wextra -Wpedantic -MMD -MP
CFLAGS := -std=c23
CXXFLAGS := -std=c++23
CPPFLAGS :=
LDFLAGS :=

DEBUG_FLAGS := -DDEBUG -g3 -ggdb -O0 -ftrapv
RELEASE_FLAGS := -DNDEBUG -O3 -mtune=native



SRCS := $(shell find $(SRC_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')
RELEASE_OBJS := $(SRCS:%=$(RELEASE_DIR)/%.o)
DEBUG_OBJS := $(SRCS:%=$(DEBUG_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INCLUDE_DIRS := $(shell find $(SRC_DIR) $(LIB_DIR) -type d)
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))
LOCAL_LIBS := $(LOCAL_LIB_NAMES:%=$(BUILD_DIR)/lib/%)



# ------------------------- Release Tasks -------------------------


.PHONY: build
build: $(RELEASE_DIR)/$(TARGET)
	@echo "make: release build finished"

.PHONY: run
run: build
	@echo "make: running release build"
	@$(RELEASE_DIR)/$(TARGET)
	@echo "make: program exited without errors"

# RELEASE: The final build step.
$(RELEASE_DIR)/$(TARGET): $(RELEASE_OBJS) $(LOCAL_LIBS)
	@echo "make: linking to target"
	@$(CXX) $(RELEASE_OBJS) $(LOCAL_LIBS) -o $@ $(LIBS) $(LDFLAGS)

# RELEASE: Build step for C source
$(RELEASE_DIR)/%.c.o: %.c
	@echo "make: compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) $(INCLUDE_FLAGS) $(COMMONFLAGS) $(CFLAGS) $(CPPFLAGS) $(RELEASE_FLAGS) -c $< -o $@

# RELEASE: Build step for C++ source
$(RELEASE_DIR)/%.cpp.o: %.cpp
	@echo "make: compiling $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(INCLUDE_FLAGS) $(COMMONFLAGS) $(CPPFLAGS) $(RELEASE_FLAGS) -c $< -o $@


# ------------------------- Debug Tasks -------------------------


.PHONY: buildDebug
buildDebug: $(DEBUG_DIR)/$(TARGET)
	@echo "make: release build finished"

.PHONY: runDebug
runDebug: buildDebug
	@echo "make: running release build"
	@$(DEBUG_DIR)/$(TARGET)
	@echo "make: program exited without errors"

# DEBUG: The final build step.
$(DEBUG_DIR)/$(TARGET): $(DEBUG_OBJS) $(LOCAL_LIBS)
	@echo "make: linking to target"
	@$(CXX) $(DEBUG_OBJS) $(LOCAL_LIBS) -o $@ $(LIBS) $(LDFLAGS)

# DEBUG: Build step for C source
$(DEBUG_DIR)/%.c.o: %.c
	@echo "make: compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) $(INCLUDE_FLAGS) $(COMMONFLAGS) $(CFLAGS) $(CPPFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# DEBUG: Build step for C++ source
$(DEBUG_DIR)/%.cpp.o: %.cpp
	@echo "make: compiling $<"
	@mkdir -p $(dir $@)
	@$(CXX) $(INCLUDE_FLAGS) $(COMMONFLAGS) $(CPPFLAGS) $(DEBUG_FLAGS) -c $< -o $@


# ------------------------- Extra -------------------------


.PHONY: install
install:
	@echo "make: installing local libs"
	@bash ./install.sh $(BUILD_DIR)/lib

.PHONY: clean
clean:
	@rm -r $(BUILD_DIR)

-include $(DEPS)
