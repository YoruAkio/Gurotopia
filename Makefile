CXX = g++
CXXFLAGS = -std=c++2b -g -Iinclude -Iinclude/ssl -MMD -MP -MF $(BUILD_DIR)/$*.d

# Build directory
BUILD_DIR := build

ifeq ($(OS),Windows_NT)
    LIBS := -L./include/enet/lib -L./include/mimalloc/lib -L./include/sqlite/lib -L./include/ssl/openssl/lib -L./include/ssl/crypto/lib
    LIBS += -lssl_32 -lcrypto_32 -lenet_32 -lws2_32 -ladvapi32 -lcrypt32 -lwinmm -lmimalloc_32 -lsqlite3_32
    TARGET_NAME := main.exe
else
    # Use system libraries on macOS/Linux
    LIBS := -L/usr/local/lib -I/usr/local/include
    LIBS += -lssl -lcrypto -lenet -lmimalloc -lsqlite3
    TARGET_NAME := main.out
    # Add Homebrew paths for macOS
    ifeq ($(shell uname),Darwin)
        CXXFLAGS += -I/usr/local/include -I/usr/local/opt/openssl@3/include
        LIBS += -L/usr/local/opt/openssl@3/lib
    endif
endif

TARGET := $(BUILD_DIR)/$(TARGET_NAME)
PCH := $(BUILD_DIR)/pch.gch

all: $(BUILD_DIR) $(PCH) $(TARGET)

# Map source files to object files in build directory
SOURCES := main.cpp $(wildcard include/**/*.cpp)
OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LIBS)

$(BUILD_DIR) :
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/include/action
	@mkdir -p $(BUILD_DIR)/include/commands
	@mkdir -p $(BUILD_DIR)/include/database
	@mkdir -p $(BUILD_DIR)/include/event_type
	@mkdir -p $(BUILD_DIR)/include/https
	@mkdir -p $(BUILD_DIR)/include/on
	@mkdir -p $(BUILD_DIR)/include/packet
	@mkdir -p $(BUILD_DIR)/include/state
	@mkdir -p $(BUILD_DIR)/include/tools
	@cp -r ctx $(BUILD_DIR)/

$(PCH): include/pch.hpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -x c++-header $< -o $@

$(BUILD_DIR)/%.o: %.cpp $(PCH) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(BUILD_DIR)
