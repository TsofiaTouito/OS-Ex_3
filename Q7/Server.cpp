#include "ConvexHull.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <csignal>
#include <pthread.h>

#define SERVER_PORT "9034" // Server's port number
#define MESSAGE_BUFFER 50  // Buffer size for client messages

using std::cout;
using std::endl;
using std::string;

// Global variables for managing the server state
int server_socket;
pthread_t connection_thread;
pthread_mutex_t data_mutex;  // Mutex for protecting shared data

std::vector<Point> point_list;
size_t remaining_points = 0;
int active_client_fd = -1;

// Signal handler to gracefully shut down the server
void signal_handler(int signal_num) {
    cout << endl << "Caught signal (" << signal_num << "), shutting down the server..." << endl;

    close(server_socket);  // Close the main server socket
    pthread_cancel(connection_thread);  // Cancel the connection thread
    pthread_join(connection_thread, nullptr);  // Wait for the thread to finish
    pthread_mutex_destroy(&data_mutex);  // Destroy the mutex

    exit(0);
}

// Processes client commands and generates appropriate responses
string execute_command(char* input, int client_fd) {
    if (remaining_points > 0) {
        if (active_client_fd != client_fd) {
            return "Another client is currently setting up the graph.";
        }

        float x, y;
        if (sscanf(input, "%f,%f", &x, &y) != 2) {
            return "Invalid format for point coordinates.";
        }
        point_list.emplace_back(x, y);
        remaining_points--;
        if (remaining_points == 0) {
            active_client_fd = -1;
            return "Graph creation completed.";
        }
        return "Point added successfully.";
    }

    if (strncmp(input, "CreateGraph", 11) == 0) {
        size_t num_points;
        if (sscanf(input, "CreateGraph %zu", &num_points) != 1 || num_points == 0) {
            return "Invalid graph creation command.";
        }

        point_list.clear();
        point_list.reserve(num_points);
        active_client_fd = client_fd;
        remaining_points = num_points;

        return "Send point coordinates to create the graph.";
    } else if (strncmp(input, "ComputeCH", 9) == 0) {
        float area = ConvexHull::convexHullArea(point_list);
        return "Convex hull area: " + std::to_string(area);
    } else if (strncmp(input, "AddPoint", 8) == 0) {
        float x, y;
        if (sscanf(input, "AddPoint %f,%f", &x, &y) != 2) {
            return "Invalid format for point coordinates.";
        }

        point_list.emplace_back(x, y);
        return "Point added successfully.";
    } else if (strncmp(input, "RemovePoint", 11) == 0) {
        float x, y;
        if (sscanf(input, "RemovePoint %f,%f", &x, &y) != 2) {
            return "Invalid format for point coordinates.";
        }

        for (size_t i = 0; i < point_list.size(); ++i) {
            if (point_list[i].getX() == x && point_list[i].getY() == y) {
                point_list[i] = point_list.back();
                point_list.pop_back();
                return "Point removed successfully.";
            }
        }
        return "Point not found.";
    } else if (strncmp(input, "GenerateRandom", 14) == 0) {
        point_list.clear();
        point_list.reserve(10000000);
        for (size_t i = 0; i < 10000000; ++i) {
            point_list.emplace_back((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
        }

        return "Random points generated.";
    }

    return "Unknown command.";
}

// Thread function for handling client messages
void* client_message_handler(void* client_fd_ptr) {
    int client_fd = *(int*)client_fd_ptr;

    while (true) {
        char buffer[MESSAGE_BUFFER] = {0};
        ssize_t bytes_received = recv(client_fd, buffer, MESSAGE_BUFFER, 0);
        if (bytes_received > 0) {
            cout << "Received from client " << client_fd << ": " << buffer;

            pthread_mutex_lock(&data_mutex);
            string response = execute_command(buffer, client_fd) + "\n>> ";
            pthread_mutex_unlock(&data_mutex);

            send(client_fd, response.c_str(), response.size(), 0);
        } else if (bytes_received == 0) {
            cout << "Client " << client_fd << " disconnected." << endl;
            close(client_fd);
            break;
        } else {
            perror("recv");
            break;
        }
    }
    return nullptr;
}

// Thread function for accepting new client connections
void* connection_handler(void* server_fd_ptr) {
    int server_fd = *(int*)server_fd_ptr;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        cout << "New client connected: " << client_fd << endl;
        send(client_fd, ">> ", 3, 0);

        pthread_t client_thread;
        if (pthread_create(&client_thread, nullptr, client_message_handler, &client_fd) != 0) {
            perror("pthread_create");
            close(client_fd);
            continue;
        }

        pthread_detach(client_thread);
    }

    return nullptr;
}

// Sets up the server listener socket
int setup_server_socket() {
    int listener;
    int reuse_addr = 1;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(nullptr, SERVER_PORT, &hints, &res) != 0) {
        return -1;
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) continue;

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) == 0) break;

        close(listener);
    }

    freeaddrinfo(res);

    if (!p || listen(listener, 10) == -1) return -1;

    return listener;
}

int main() {
    signal(SIGINT, signal_handler);

    if (pthread_mutex_init(&data_mutex, nullptr) != 0) {
        perror("Failed to initialize mutex.");
        return 1;
    }

    server_socket = setup_server_socket();
    if (server_socket == -1) {
        perror("Failed to initialize server socket.");
        return 1;
    }

    if (pthread_create(&connection_thread, nullptr, connection_handler, &server_socket) != 0) {
        perror("Failed to create connection thread.");
        return 1;
    }

    pthread_join(connection_thread, nullptr);
    pthread_mutex_destroy(&data_mutex);

    return 0;
}
