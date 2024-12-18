#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9034
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/Address not supported" << endl;
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection failed" << endl;
        return -1;
    }

    while (true) {
        cout << "Enter command (or 'exit' to quit): ";
        string command;
        getline(cin, command);

        if (command == "exit") {
            cout << "Exiting client." << endl;
            break;
        }

        send(sock, command.c_str(), command.length(), 0);
        read(sock, buffer, BUFFER_SIZE);

        cout << "Server response: " << buffer << endl;
        memset(buffer, 0, BUFFER_SIZE);
    }

    close(sock);
    return 0;
}
