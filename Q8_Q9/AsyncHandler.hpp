#ifndef NETWORK_ASYNC_HANDLER_HPP
#define NETWORK_ASYNC_HANDLER_HPP

#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>

#define MAX_CLIENT_EVENTS 10

/**
 * @brief Function pointer type for handling events in a Reactor.
 */
typedef void (*EventHandler)(int fd);

/**
 * @class AsyncReactor
 * @brief Handles events for multiple file descriptors using a polling mechanism.
 */
class AsyncReactor {
private:
    bool active;
    struct pollfd* eventFds;
    unsigned int eventCount;
    std::unordered_map<int, EventHandler> eventHandlers;

public:
    AsyncReactor();
    ~AsyncReactor();

    void addFileDescriptor(int fd, EventHandler handler);
    void removeFileDescriptor(int fd);
    void start();
    void stop();
};

/**
 * @brief Function pointer type for handling client requests in a Proactor.
 */
typedef void* (*ClientHandler)(int, std::mutex&);

/**
 * @class AsyncProactor
 * @brief Handles asynchronous client connections and interactions.
 */
class AsyncProactor {
private:
    std::atomic<bool> active;
    int serverSocketFd;
    std::thread connectionThread;
    std::mutex handlerMutex;

public:
    AsyncProactor();
    ~AsyncProactor();

    void start(int socketFd, ClientHandler clientHandler);
    void shutdown();

private:
    // Utility function to get the network address (IPv4 or IPv6):
    void* extractAddress(struct sockaddr* sa);
};

#endif // NETWORK_ASYNC_HANDLER_HPP
