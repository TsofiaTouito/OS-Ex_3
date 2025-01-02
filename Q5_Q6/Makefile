CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++17 -g -fPIC

LDFLAGS = -shared



MAIN = Server.cpp

SRCS = Point.cpp ConvexHull.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = server

LIBRARY = libreactor.so



all: $(TARGET)



$(TARGET): $(OBJS) $(MAIN:.cpp=.o) $(LIBRARY)

	$(CXX) $(CXXFLAGS) -o $@ $^ -L. -lreactor -Wl,-rpath=.



$(LIBRARY): Reactor.o

	$(CXX) $(LDFLAGS) -o $@ $^



%.o: %.cpp

	$(CXX) $(CXXFLAGS) -c $< -o $@



clean:

	rm -f *.o $(TARGET) $(LIBRARY)



.PHONY: all clean
