# Makefile for compiling either the list or deque version

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -g -pg

# Object files and executable
EXEC := program

# Default target (can be 'deque' or 'list')
all: deque

# Target for deque implementation
deque: Q1_deque.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) Q1_deque.o

# Target for list implementation
list: Q1_list.o
	$(CXX) $(CXXFLAGS) -o $(EXEC) Q1_list.o

# Compile Q1_deque.cpp into an object file
Q1_deque.o: Q1_deque.cpp
	$(CXX) $(CXXFLAGS) -c Q1_deque.cpp

# Compile Q1_list.cpp into an object file
Q1_list.o: Q1_list.cpp
	$(CXX) $(CXXFLAGS) -c Q1_list.cpp

# Clean up build files
clean:
	rm -f *.o $(EXEC)
