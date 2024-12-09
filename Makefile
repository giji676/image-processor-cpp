CC = g++
CXX_STANDARD = -std=c++17
LIBS =  # Add necessary libraries here, e.g., -lm
BUILD_DIR = build/
SRC_DIR = src/
INCLUDE_DIR = include/
SOURCES = $(wildcard $(SRC_DIR)*.cpp)
HEADERS = $(wildcard $(INCLUDE_DIR)*.h)
OBJECTS = $(SOURCES:$(SRC_DIR)%.cpp=$(BUILD_DIR)%.o)
TARGET = $(BUILD_DIR)main

# Compiler flags
CXXFLAGS = $(CXX_STANDARD) -I$(INCLUDE_DIR)

# Default target
all: $(TARGET)

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link object files to create the final executable
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) $(LIBS) -o $@

# Compile source files into object files
$(BUILD_DIR)%.o: $(SRC_DIR)%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CXXFLAGS) -c $< -o $@

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)
