#include "Reactor.hpp"

Reactor::Reactor() : running(false), eventPollFdsSize(0) {
    eventPollFds = new struct pollfd[MAX_EVENT_COUNT];
    eventCallbackMap.reserve(MAX_EVENT_COUNT);
}

Reactor::~Reactor() {
    running = false;
    for (unsigned int i = 0; i < eventPollFdsSize; i++)
        close(eventPollFds[i].fd);
    delete[] eventPollFds;
}

void Reactor::registerFd(int fd, EventCallback callback) {
    eventPollFds[eventPollFdsSize++] = {fd, POLLIN, 0};
    eventCallbackMap[fd] = callback;
}

void Reactor::unregisterFd(int fd) {
    unsigned int index = MAX_EVENT_COUNT + 1;
    for (unsigned int i = 0; i < eventPollFdsSize; i++)
        if (eventPollFds[i].fd == fd) { index = i; break; }
    if (index == MAX_EVENT_COUNT + 1) return;

    eventPollFds[index] = eventPollFds[eventPollFdsSize - 1];
    eventPollFdsSize--;
    eventCallbackMap.erase(fd);
}

void Reactor::start() {
    running = true;

    while (running) {
        int eventCount = poll(eventPollFds, eventPollFdsSize, 10);

        if (eventCount == -1) {
            if (errno == EINTR) { continue; }
            else { perror("poll"); break; }
        } else if (eventCount > 0) {
            for (unsigned int i = 0; i < eventPollFdsSize; i++) {
                if (eventPollFds[i].revents & POLLIN) {
                    eventPollFds[i].revents = 0; // Clear the event flag after handling
                    eventCallbackMap[eventPollFds[i].fd](eventPollFds[i].fd);
                }
            }
        }
    }
}

void Reactor::halt() {
    running = false;
}
