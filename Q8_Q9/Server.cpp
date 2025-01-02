#include "ConvexHull.hpp"
#include "AsyncHandler.hpp"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define SERVER_PORT "9034"
#define MAX_EVENTS 10
#define MSG_BUFFER_SIZE 50

using std::cout;
using std::endl;
using std::string;

// Event-driven reactor for handling async operations
AsyncReactor asyncReactor;

// Container for graph points
std::vector<Point> graphPoints;

// Tracks the number of points expected during graph creation
size_t pendingPoints = 0;

// File descriptor of the client creating the graph
int graphCreatorFd = -1;

/**
 * @brief Signal handler for SIGINT to gracefully shut down the server.
 */
void signalHandler(int signal) {
    cout << "\nReceived SIGINT (" << signal << "), shutting down server..." << endl;
    asyncReactor.stop();
}

/**
 * @brief Processes client commands and manages graph operations.
 * @param inputLine The command received from the client.
 * @param clientFd The file descriptor of the client sending the command.
 * @return A response string for the client.
 */
string handleClientCommand(char* inputLine, int clientFd) {
    if (pendingPoints > 0) {
        if (graphCreatorFd != clientFd)
            return "Another client is creating a graph";

        float x, y;
        if (sscanf(inputLine, "%f,%f", &x, &y) != 2)
            return "Invalid coordinates format while waiting for points";
        graphPoints.emplace_back(x, y);
        pendingPoints--;
        if (pendingPoints == 0) {
            graphCreatorFd = -1;
            return "Graph creation complete";
        }
        return "Point added";
    }

    if (strncmp(inputLine, "CreateGraph", 11) == 0) {
        size_t pointCount;
        if (sscanf(inputLine, "CreateGraph %zu", &pointCount) != 1)
            return "Invalid CreateGraph command format";
        if (pointCount == 0)
            return "Graph must have at least one point";

        graphPoints.clear();
        graphPoints.reserve(pointCount);
        graphCreatorFd = clientFd;
        pendingPoints = pointCount;
        return "Expecting points for new graph";
    } else if (strncmp(inputLine, "CH", 2) == 0) {
        float hullArea = 0;
        if (graphPoints.size() > 2)
            hullArea = ConvexHullUtility::computeHullArea(graphPoints);
        return "Convex hull area: " + std::to_string(hullArea);
    } else if (strncmp(inputLine, "AddPoint", 8) == 0) {
        float x, y;
        if (sscanf(inputLine, "AddPoint %f,%f", &x, &y) != 2)
            return "Invalid coordinates format";
        graphPoints.emplace_back(x, y);
        return "Point added";
    } else if (strncmp(inputLine, "RemovePoint", 11) == 0) {
        float x, y;
        if (sscanf(inputLine, "RemovePoint %f,%f", &x, &y) != 2)
            return "Invalid coordinates format";

        for (unsigned int i = 0; i < graphPoints.size(); i++) {
            if (graphPoints[i].getX() == x && graphPoints[i].getY() == y) {
                graphPoints[i] = graphPoints.back();
                graphPoints.pop_back();
                break;
            }
        }
        return "Point removed";
    } else if (strncmp(inputLine, "GenerateRandom", 14) == 0) {
        graphPoints.clear();
        graphPoints.reserve(10000000);
        for (size_t i = 0; i < 10000000; i++) {
            graphPoints.emplace_back((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
        }
        return "Random points generated";
    }

    return "Unknown command";
}

/**
 * @brief Creates a listening socket for the server.
 * @return The file descriptor of the listening socket, or -1 on error.
 */
int createServerSocket(void) {
    int serverSocket;
    int optval = 1;
    int status;

    struct addrinfo hints, *serverInfo, *current;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, SERVER_PORT, &hints, &serverInfo)) != 0) {
        fprintf(stderr, "Server error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (current = serverInfo; current != NULL; current = current->ai_next) {
        serverSocket = socket(current->ai_family, current->ai_socktype, current->ai_protocol);
        if (serverSocket < 0) {
            continue;
        }

        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));

        if (bind(serverSocket, current->ai_addr, current->ai_addrlen) < 0) {
            close(serverSocket);
            continue;
        }

        break;
    }

    freeaddrinfo(serverInfo);

    if (current == NULL) {
        return -1;
    }

    if (listen(serverSocket, MAX_EVENTS) == -1) {
        return -1;
    }

    return serverSocket;
}

/**
 * @brief Handles messages from clients and processes their commands.
 * @param clientFd The file descriptor of the client socket.
 * @param mutex Mutex for synchronizing command handling.
 */
void* processClientMessages(int clientFd, std::mutex &mutex) {
    char messageBuffer[MSG_BUFFER_SIZE] = {0};
    ssize_t receivedBytes;

    while ((receivedBytes = recv(clientFd, messageBuffer, MSG_BUFFER_SIZE - 1, 0)) > 0) {
        messageBuffer[receivedBytes] = '\0';
        std::cout << "Message from client " << clientFd << ": " << messageBuffer;

        std::string response;
        mutex.lock();
        response = handleClientCommand(messageBuffer, clientFd) + "\n>> ";
        mutex.unlock();

        send(clientFd, response.c_str(), response.size(), 0);
        memset(messageBuffer, 0, sizeof(messageBuffer));
    }

    if (receivedBytes == 0) {
        std::cout << "Client " << clientFd << " disconnected." << std::endl;
        close(clientFd);
    } else {
        perror("recv");
    }
    return nullptr;
}

int main() {
    int serverSocket = createServerSocket();
    if (serverSocket == -1) {
        perror("Error creating server socket");
        return 1;
    }

    asyncReactor.addFileDescriptor(serverSocket, [](int fd) {
    std::mutex mutex; // Adjust if needed
    processClientMessages(fd, mutex); // Wrap in lambda for additional arguments
});

    std::cout << "Server started, listening on port " << SERVER_PORT << std::endl;
    asyncReactor.start();

    return 0;
}
