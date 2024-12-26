#include "Reactor.hpp"




// Starts a new reactor and returns a pointer to it
void* startReactor() {
    Reactor* reactor = new Reactor();
    memset(reactor->reactorFuncs, 0, sizeof(reactor->reactorFuncs));
    return reactor;
}


// Adds an fd to the reactor for reading; returns 0 on success
int addFdToReactor(void* reactorPtr, int fd, reactorFunc func) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);
    reactor->fds.insert(fd);
    reactor->reactorFuncs[fd] = func;
    return 0;
}


// Removes an FD from the reactor
int removeFdFromReactor(void* reactorPtr, int fd) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);
    reactor->fds.erase(fd);
    reactor->reactorFuncs[fd] = nullptr;
    return 0;
}


// Stops the reactor and frees memory
int stopReactor(void* reactorPtr) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);
    delete reactor;
    return 0;
}



// Runs the reactor event loop
void runReactor(void* reactorPtr) {
    Reactor* reactor = static_cast<Reactor*>(reactorPtr);

    while (true) {
        fd_set readfds;
        FD_ZERO(&readfds);

        int maxfd = 0;
        for (int fd : reactor->fds) {
            FD_SET(fd, &readfds);
            if (fd > maxfd) maxfd = fd;
        }

        struct timeval tv = {1, 0}; // 1-second timeout
        int activity = select(maxfd + 1, &readfds, nullptr, nullptr, &tv);

        if (activity < 0) {
            std::cerr << "Error in select\n";
            break;
        }

        for (int fd : reactor->fds) {
            if (FD_ISSET(fd, &readfds)) {
                if (reactor->reactorFuncs[fd]) {
                    reactor->reactorFuncs[fd](fd);
                }
            }
        }
    }
}