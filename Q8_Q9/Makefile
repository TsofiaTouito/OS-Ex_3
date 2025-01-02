CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g -fPIC
LDFLAGS = -shared

MAIN = Server.cpp
SRCS = Point.cpp ConvexHull.cpp AsyncReactor.cpp AsyncProactor.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = server

HEADERS = AsyncHandler.hpp
LIBRARY = libasynchandling.so
LIBRARY_OBJS = Reactor.o Proactor.o AsyncReactor.o AsyncProactor.o

all: $(TARGET)

$(TARGET): $(OBJS) $(MAIN:.cpp=.o) $(LIBRARY)
	$(CXX) $(CXXFLAGS) -o $@ $^ -L. -lasynchandling -Wl,-rpath=.

$(LIBRARY): $(LIBRARY_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(LIBRARY)

.PHONY: all clean
