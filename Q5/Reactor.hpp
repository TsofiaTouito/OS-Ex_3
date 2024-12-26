#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <iostream>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <cstring>
#include <set>



// Type definition for the reactor function
typedef void* (* reactorFunc) (int fd);

// Reactor structure definition
struct Reactor {
    std::set<int> fds;                 // File descriptors
    reactorFunc reactorFuncs[FD_SETSIZE]; // Callback functions
};


// starts new reactor and returns pointer to it 
void *startReactor(); 

// adds fd to Reactor (for reading) ; returns 0 on success.
int addFdToReactor(void * reactor, int fd, reactorFunc func);

int addFdToReactor(void * reactor, int fd, reactorFunc func); 

// removes fd from reactor
int removeFdFromReactor(void * reactor, int fd); 

// stops reactor
int stopReactor(void * reactor);


#endif