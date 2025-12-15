CXX = g++
# Removed -lX11 since we are using cimg_display 0 (No GUI)
# Added -lm for math library, though usually implicit
LDFLAGS = -lpthread -lm 

# Flags: O2 for optimization, C++11 standard
CXXFLAGS = -O2 -std=c++11 -pthread

# Source files
SRCS = src/main.cpp src/Morphology.cpp src/Segmentation.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = task3

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
