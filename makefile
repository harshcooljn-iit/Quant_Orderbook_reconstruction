# Compiler
CXX = g++
CXXFLAGS = -std=c++23 -O3 -Wall -Wextra -Wpedantic -march=native -flto -ffast-math
LDFLAGS = -flto

# Source files
SRCS = main.cpp order.cpp
OBJS = $(SRCS:.cpp=.o)

# Executable name
TARGET = reconstruction

# Default rule
all: $(TARGET)

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile each source file into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Remove build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Use -j for parallel builds
