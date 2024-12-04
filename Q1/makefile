OUTPUT_LIST = program_list
OUTPUT_DEQUE = program_deque

SOURCES_LIST = Q1_list.cpp    
SOURCES_DEQUE = Q1_deque.cpp    

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -g   

# Default target
all: list     

# Compile and link the deque implementation
deque: $(SOURCES_DEQUE) 
	g++ $(CXXFLAGS) -pg -o $(OUTPUT_DEQUE) $(SOURCES_DEQUE) 

# Compile and link the list implementation
list: $(SOURCES_LIST)
	g++ $(CXXFLAGS) -pg -o $(OUTPUT_LIST) $(SOURCES_LIST) 

# Clean generated files
clean:
	rm -f $(OUTPUT_LIST) $(OUTPUT_DEQUE) gmon.out 
