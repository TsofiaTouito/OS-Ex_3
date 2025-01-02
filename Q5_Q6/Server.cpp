#include "ConvexHull.hpp"
#include "Reactor.hpp"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>

#define PORT "9034" // Port number for the server
#define BUFFER_SIZE 50 // Buffer size for client messages

using std::cout;
using std::endl;
using std::string;

// Global variables for server state
Reactor reactor;               // Reactor for managing I/O events
std::vector<Point> graphPoints; // Points representing the graph
size_t pointsRemaining = 0;    // Number of points yet to be received
int creatorClientFd = -1;      // File descriptor of the graph creator client

// Signal handler to shut down the server gracefully
void handleSignalInterrupt(int signal) {
    cout << "\nReceived SIGINT (signal " << signal << "), shutting down the server..." << endl;
    reactor.halt();
}

// Processes client commands and returns a response
string processClientCommand(char* input, int clientFd) {
    if (pointsRemaining > 0) {
        // Handle point addition during graph creation
        if (creatorClientFd != clientFd) {
            return "Another client is creating a graph";
        }

        float x, y;
        if (sscanf(input, "%f,%f", &x, &y) != 2) {
            return "Invalid coordinates format while waiting for points";
        }

        graphPoints.emplace_back(x, y);
        pointsRemaining--;
        if (pointsRemaining == 0) {
            creatorClientFd = -1;
            return "Graph creation complete";
        }
        return "Point added";
    }

    // Handle specific commands
    if (strncmp(input, "Newgraph", 8) == 0) {
        size_t numPoints;
        if (sscanf(input, "Newgraph %zu", &numPoints) != 1 || numPoints == 0) {
            return "Invalid Newgraph command or graph size must be at least 1";
        }

        graphPoints.clear();
        graphPoints.reserve(numPoints);
        creatorClientFd = clientFd;
        pointsRemaining = numPoints;
        return "Expecting points for new graph";
    } else if (strncmp(input, "CH", 2) == 0) {
        float convexHullArea = ConvexHullUtility::computeHullArea(graphPoints);
        return "Convex hull area: " + std::to_string(convexHullArea);
    } else if (strncmp(input, "Newpoint", 8) == 0) {
        float x, y;
        if (sscanf(input, "Newpoint %f,%f", &x, &y) != 2) {
            return "Invalid coordinates format";
        }

        graphPoints.emplace_back(x, y);
        return "Point added";
    } else if (strncmp(input, "Removepoint", 11) == 0) {
        float x, y;
        if (sscanf(input, "Removepoint %f,%f", &x, &y) != 2) {
            return "Invalid coordinates format";
        }

        for (size_t i = 0; i < graphPoints.size(); ++i) {
            if (graphPoints[i].getX() == x && graphPoints[i].getY() == y) {
                graphPoints[i] = graphPoints.back();
                graphPoints.pop_back();
                return "Point removed";
            }
        }
        return "Point not found";
    }

    return "Unknown command";
}

// Retrieves the appropriate address (IPv4 or IPv6) from a sockaddr structure
void* getInAddress(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Creates and returns a listening socket
int createListenerSocket() {
    int listener;
    int yes = 1;
    int result;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((result = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "Server error: %s\n", gai_strerror(result));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai);

    if (p == NULL || listen(listener, SOMAXCONN) == -1) {
        return -1;
    }

    return listener;
}

// Handles incoming messages from clients
void handleClientMessage(int clientFd) {
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytesRead = recv(clientFd, buffer, BUFFER_SIZE, 0);

    if (bytesRead > 0) {
        cout << "Message from client " << clientFd << ": " << buffer;
        string response = processClientCommand(buffer, clientFd) + "\n>> ";
        send(clientFd, response.c_str(), response.size(), 0);
    } else if (bytesRead == 0) {
        cout << "Client " << clientFd << " disconnected." << endl;
        close(clientFd);
        reactor.unregisterFd(clientFd);
    } else {
        perror("recv");
    }
}

// Handles new client connections
void handleNewConnection(int serverFd) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int newClientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (newClientFd == -1) {
        perror("accept");
        return;
    }

    cout << "New client connected: " << newClientFd << endl;
    send(newClientFd, ">> ", 3, 0);
    reactor.registerFd(newClientFd, handleClientMessage);
}

int main() {
    signal(SIGINT, handleSignalInterrupt);

    int listener = createListenerSocket();
    if (listener == -1) {
        perror("Error creating listener socket");
        return 1;
    }

    reactor.registerFd(listener, [](int listenerFd) { handleNewConnection(listenerFd); });

    cout << "Server started, listening on port " << PORT << endl;
    reactor.start();

    return 0;
}
