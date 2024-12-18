# Makefile for server and client

CXX = g++
CXXFLAGS = -Wall -std=c++17
TARGETS = server client

all: $(TARGETS)

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

clean:
	rm -f $(TARGETS)
