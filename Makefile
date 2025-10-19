# Compiler settings for Windows MinGW
CXX := g++
CXXFLAGS := -std=gnu++11 -O2 -Wall
INCLUDES := -Isrc

# Source files
SOURCES := src/main.cpp \
           src/Utils.cpp \
           src/ImageProcessor.cpp \
           src/Operations.cpp \
           src/Geometric.cpp \
           src/NoiseFilters.cpp \
           src/Metrics.cpp

# Output target
TARGET := build/imageProcessor.exe
LIBS := -lgdi32

# Build target
all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo Building image processor...
	@if not exist build mkdir build
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)
	@echo Build complete: $(TARGET)

# Clean
clean:
	@if exist build\imageProcessor.exe del /Q build\imageProcessor.exe
	@echo Clean complete

# Rebuild
rebuild: clean all

# Help
help:
	@echo Available targets:
	@echo   mingw32-make          - Build the project
	@echo   mingw32-make clean    - Remove compiled binary
	@echo   mingw32-make rebuild  - Clean and rebuild
	@echo   mingw32-make help     - Show this message

.PHONY: all clean rebuild help
