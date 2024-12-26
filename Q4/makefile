# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Target executable
TARGET = server

# Source files
SRCS = Graph.cpp Server.cpp

# Header files
HDRS = Graph.hpp Point.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
