CC = g++
CXX_STANDARD = -std=c++17  # Replace with -std=c++11, -std=c++14, -std=c++20 as needed
LIBS =
BUILD_DIR = build/
SOURCES = main.cpp imageProcessor.cpp
HEADERS = pixel.h imageProcessor.h
TARGET = main

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

all: $(BUILD_DIR) $(BUILD_DIR)$(TARGET)

$(BUILD_DIR)$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CXX_STANDARD) $(SOURCES) $(LIBS) -o $(BUILD_DIR)$(TARGET)

clean:
	rm -rf $(BUILD_DIR)/*
