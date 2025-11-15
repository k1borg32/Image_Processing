# Compiler settings for macOS
CXX := clang++
CXXFLAGS := -std=c++11 -O2 -Wall -Wextra
INCLUDES := -Isrc

# macOS doesn't need special display libraries for BMP files
# CImg is header-only, so no linking required for basic BMP support
LIBS := 

# Source files
SOURCES := src/main.cpp \
           src/Utils.cpp \
           src/ImageProcessor.cpp \
           src/Operations.cpp \
           src/Geometric.cpp \
           src/NoiseFilters.cpp \
           src/Metrics.cpp

# Output target
TARGET := build/imageProcessor
BUILD_DIR := build

# Build target
all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo "Building image processor..."
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)
	@echo "Build complete: $(TARGET)"

# Clean
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

# Rebuild
rebuild: clean all

# Run the program (example)
run: $(TARGET)
	./$(TARGET) --help

# Help
help:
	@echo "Available targets:"
	@echo "  make          - Build the project"
	@echo "  make clean    - Remove compiled binary"
	@echo "  make rebuild  - Clean and rebuild"
	@echo "  make run      - Build and run with --help"
	@echo "  make help     - Show this message"

.PHONY: all clean rebuild run help
