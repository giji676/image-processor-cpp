CC = g++
CXX_STANDARD = -std=c++17  # Replace with -std=c++11, -std=c++14, -std=c++20 as needed
LIBS =
BUILD_DIR = build/
FILENAME = main

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

all:
	$(CC) $(CXX_STANDARD) $(FILENAME).cpp $(LIBS) -o $(BUILD_DIR)$(basename $(FILENAME))
clean:
	rm -rf $(BUILD_DIR)/*
