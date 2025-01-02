#include "AsyncHandler.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

AsyncProactor::AsyncProactor() : active(false), serverSocketFd(-1) {}

AsyncProactor::~AsyncProactor() {
    shutdown();
}

void AsyncProactor::start(int socketFd, ClientHandler clientHandler) {
    active = true;
    serverSocketFd = socketFd;

    // Start a thread to handle new client connections
    connectionThread = std::thread([this, clientHandler] {
        struct sockaddr_storage clientAddress;
        socklen_t addressSize = sizeof(clientAddress);

        while (active) {
            int clientSocketFd = accept(serverSocketFd, (struct sockaddr*)&clientAddress, &addressSize);
            if (clientSocketFd == -1) {
                if (!active) break; // Exit if the server is stopping
                perror("accept");
                continue;
            }

            // Log the client connection
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(clientAddress.ss_family, extractAddress((struct sockaddr*)&clientAddress), 
                      addressBuffer, sizeof(addressBuffer));
            std::cout << "Server: received connection from " << addressBuffer << std::endl;
            send(clientSocketFd, ">> ", 3, 0);

            // Spawn a new thread to handle the client
            std::thread clientThread([clientSocketFd, clientHandler, mutexRef = std::ref(handlerMutex)] {
                clientHandler(clientSocketFd, mutexRef);
            });
            clientThread.detach();
        }
    });

    if (connectionThread.joinable()) {
        connectionThread.join();
    }
}

void AsyncProactor::shutdown() {
    if (active) {
        active = false;
        close(serverSocketFd);
        serverSocketFd = -1;
    }
}

void* AsyncProactor::extractAddress(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
