CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -fPIC
LDFLAGS = -shared

MAIN = Server.cpp
SRCS = Point.cpp ConvexHull.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS) $(MAIN:.cpp=.o)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(LIBRARY)

.PHONY: all clean

