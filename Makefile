BUILD_DIR := build
TARGET_NAME := imageProcessor
SOURCES := src/main.cpp \
           src/Histogram.cpp \
           src/LinearFilters.cpp \
           src/NonLinearFilters.cpp \
           src/Utils.cpp
INCLUDES := -Isrc
CXXFLAGS := -std=gnu++11 -O2 -Wall -Wextra

ifeq ($(OS),Windows_NT)
    EXE := .exe
    CXX := g++
    TARGET := $(BUILD_DIR)/$(TARGET_NAME)$(EXE)
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    RMDIR = if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
    LIBS := -lgdi32
else
    EXE :=
    CXX ?= clang++
    TARGET := $(BUILD_DIR)/$(TARGET_NAME)
    MKDIR = mkdir -p $(BUILD_DIR)
    RMDIR = rm -rf $(BUILD_DIR)
    LIBS :=
endif

.PHONY: all clean rebuild run help

all: $(TARGET)

$(TARGET): $(SOURCES)
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)
	@echo Build complete: $(TARGET)

clean:
	@$(RMDIR)
	@echo Clean complete

rebuild: clean all

run: $(TARGET)
	$(TARGET) --help

help:
	@echo Available targets:
	@echo   make          - Build the project
	@echo   make clean    - Remove compiled binary
	@echo   make rebuild  - Clean and rebuild
	@echo   make run      - Build and run with --help
	@echo   make help     - Show this message
