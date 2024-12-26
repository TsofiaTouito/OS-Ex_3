#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <fcntl.h>
#include "Graph.hpp" // Header file for the Graph class

#define PORT 9034
#define BACKLOG 10
#define BUFFER_SIZE 1024

// Global Graph object (shared by all clients)
Graph currentGraph;

// Function to handle client commands
void handleCommand(int client_fd, const std::string& command) {
    std::string response;

    try {
        if (command.rfind("NewGraph", 0) == 0) {
            int n;
            std::istringstream ss(command.substr(9)); // Extract size from the command
            ss >> n;

            currentGraph = Graph(); // Reset the graph
            response = "Graph cleared. Please send " + std::to_string(n) + " points in the format x,y.\n";

        } else if (command.rfind("NewPoint", 0) == 0) {
            float x, y;
            char comma;
            std::istringstream ss(command.substr(9)); // Extract point data
            ss >> x >> comma >> y;

            currentGraph.addPoint(x, y); // Add point to graph
            response = "Point (" + std::to_string(x) + "," + std::to_string(y) + ") added.\n";

        } else if (command.rfind("RemovePoint", 0) == 0) {
            float x, y;
            char comma;
            std::istringstream ss(command.substr(12)); // Extract point data
            ss >> x >> comma >> y;

            currentGraph.removePoint(x, y); // Remove point from graph
            response = "Point (" + std::to_string(x) + "," + std::to_string(y) + ") removed.\n";

        } else if (command.rfind("CH", 0) == 0) {
            auto convexHull = currentGraph.convexHull(); // Calculate convex hull
            response = "Convex Hull points:\n";
            for (const auto& point : convexHull) {
                response += "(" + std::to_string(point->getX()) + "," + std::to_string(point->getY()) + ")\n";
            }

        } else {
            response = "Invalid command.\n";
        }
    } catch (const std::exception& e) {
        response = "Error: ";
        response += e.what();
        response += "\n";
    }

    send(client_fd, response.c_str(), response.length(), 0);
}


// Function to set up and run the server
void setupServer() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking mode
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    std::vector<int> clientSockets;

    // Main loop to handle connections and commands
    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);

        int max_fd = server_fd;
        for (int client_fd : clientSockets) {
            FD_SET(client_fd, &readfds);
            max_fd = std::max(max_fd, client_fd);
        }

        int activity = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            perror("Select error");
            continue;
        }

        // Handle new client connections
        if (FD_ISSET(server_fd, &readfds)) {
            int client_fd = accept(server_fd, nullptr, nullptr);
            if (client_fd < 0) {
                perror("Accept failed");
            } else {
                clientSockets.push_back(client_fd);
                std::cout << "New client connected: " << client_fd << std::endl;
            }
        }

        // Handle commands from existing clients
        for (auto it = clientSockets.begin(); it != clientSockets.end();) {
            int client_fd = *it;
            if (FD_ISSET(client_fd, &readfds)) {
                char buffer[BUFFER_SIZE];
                memset(buffer, 0, sizeof(buffer));
                int bytesRead = recv(client_fd, buffer, sizeof(buffer), 0);

                if (bytesRead <= 0) {
                    std::cout << "Client disconnected: " << client_fd << std::endl;
                    close(client_fd);
                    it = clientSockets.erase(it);
                    continue;
                }

                std::string command(buffer);
                handleCommand(client_fd, command);
            }
            ++it;
        }
    }

    close(server_fd);
}

int main() {
    setupServer();
    return 0;
}
