#include "AsyncHandler.hpp"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

/**
 * @brief Constructor for AsyncReactor, initializes internal structures for managing events.
 */
AsyncReactor::AsyncReactor() : active(false), eventCount(0) {
    eventFds = new struct pollfd[MAX_CLIENT_EVENTS];
    eventHandlers.reserve(MAX_CLIENT_EVENTS);
}

/**
 * @brief Destructor for AsyncReactor, releases resources and closes all file descriptors.
 */
AsyncReactor::~AsyncReactor() {
    active = false;
    for (unsigned int i = 0; i < eventCount; i++) {
        close(eventFds[i].fd);
    }
    delete[] eventFds;
}

/**
 * @brief Adds a new file descriptor to be monitored for events.
 * @param fd The file descriptor to monitor.
 * @param handler The event handler function to call when the file descriptor is ready.
 */
void AsyncReactor::addFileDescriptor(int fd, EventHandler handler) {
    eventFds[eventCount++] = {fd, POLLIN, 0};
    eventHandlers[fd] = handler;
}

/**
 * @brief Removes a file descriptor from monitoring and cleans up associated resources.
 * @param fd The file descriptor to remove.
 */
void AsyncReactor::removeFileDescriptor(int fd) {
    unsigned int index = MAX_CLIENT_EVENTS + 1;
    for (unsigned int i = 0; i < eventCount; i++) {
        if (eventFds[i].fd == fd) {
            index = i;
            break;
        }
    }
    if (index == MAX_CLIENT_EVENTS + 1) return;

    // Replace the removed fd with the last fd in the array
    eventFds[index] = eventFds[eventCount - 1];
    eventCount--;
    eventHandlers.erase(fd);
}

/**
 * @brief Starts the reactor loop, polling for events and dispatching handlers.
 */
void AsyncReactor::start() {
    active = true;

    while (active) {
        int readyEvents = poll(eventFds, eventCount, -1);

        if (readyEvents == -1) {
            if (errno == EINTR) {
                continue;  // Interrupted system call, retry
            } else {
                perror("poll");
                break;
            }
        }

        // Process all triggered events
        if (readyEvents > 0) {
            for (unsigned int i = 0; i < eventCount; i++) {
                if (eventFds[i].revents & POLLIN) {
                    eventFds[i].revents = 0; // Clear the event flag
                    eventHandlers[eventFds[i].fd](eventFds[i].fd);
                }
            }
        }
    }
}

/**
 * @brief Stops the reactor loop.
 */
void AsyncReactor::stop() {
    active = false;
}
