#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <unordered_map>
#include <vector>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>

#define MAX_EVENT_COUNT 10

typedef void (*EventCallback)(int fd);

/**
 * @brief EventDispatcher class handles asynchronous event processing.
 * It uses a poll-based event loop to monitor multiple file descriptors.
 */
class Reactor {
private:
    bool running;
    struct pollfd* eventPollFds;
    unsigned int eventPollFdsSize;
    std::unordered_map<int, EventCallback> eventCallbackMap;

public:
    Reactor();
    ~Reactor();

    /**
     * @brief Register a new file descriptor and its associated callback function.
     * 
     * @param fd File descriptor to monitor.
     * @param callback Function to be called when the event occurs.
     */
    void registerFd(int fd, EventCallback callback);

    /**
     * @brief Unregister a file descriptor, removing it from monitoring.
     * 
     * @param fd File descriptor to remove.
     */
    void unregisterFd(int fd);

    /**
     * @brief Start the event loop, processing events until stopped.
     */
    void start();

    /**
     * @brief Stop the event loop.
     */
    void halt();
};

#endif // REACTOR_HPP
