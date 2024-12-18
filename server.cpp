#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define PORT 9034
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

using namespace std;

// ---------------------------------------------Graph & Point Classes--------------------------------------------

class Point {
private:
    int x, y;

public:
    Point(int x, int y) : x(x), y(y) {}
    int getX() const { return x; }
    int getY() const { return y; }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    friend ostream& operator<<(ostream& os, const Point& point) {
        os << "(" << point.x << ", " << point.y << ")";
        return os;
    }
};

class Graph {
public:
    vector<Point*> points;

    void addPoint(int x, int y) {
        points.push_back(new Point(x, y));
    }

    void removePoint(int x, int y) {
        auto it = remove_if(points.begin(), points.end(), [x, y](Point* point) {
            if (point->getX() == x && point->getY() == y) {
                delete point; // Free memory
                return true;
            }
            return false;
        });
        points.erase(it, points.end());
    }

    vector<Point*> getConvexHull() {
        // Convex Hull logic
        if (points.size() < 3)
            return points;

        auto origin_iter = min_element(points.begin(), points.end(), [](Point* a, Point* b) {
            return (a->getY() < b->getY()) || (a->getY() == b->getY() && a->getX() < b->getX());
        });

        Point origin = **origin_iter;
        sort(points.begin(), points.end(), [&origin](Point* a, Point* b) {
            float dx1 = a->getX() - origin.getX();
            float dx2 = b->getX() - origin.getX();
            float dy1 = a->getY() - origin.getY();
            float dy2 = b->getY() - origin.getY();
            return atan2(dy1, dx1) < atan2(dy2, dx2);
        });

        vector<Point*> hull;
        for (auto point : points) {
            while (hull.size() >= 2) {
                Point* p2 = hull.back();
                Point* p1 = hull[hull.size() - 2];
                float det = (p2->getX() - p1->getX()) * (point->getY() - p1->getY()) -
                            (p2->getY() - p1->getY()) * (point->getX() - p1->getX());
                if (det <= 0)
                    hull.pop_back();
                else
                    break;
            }
            hull.push_back(point);
        }
        return hull;
    }

    ~Graph() {
        for (Point* point : points)
            delete point;
    }
};

Graph sharedGraph; // Global shared graph

// ---------------------------------------------Server Logic--------------------------------------------

void processCommand(const string& command, string& response) {
    stringstream ss(command);
    string action;
    ss >> action;

    if (action == "NewGraph") {
        sharedGraph = Graph();
        response = "New graph created.";
    } else if (action == "NewPoint") {
        int x, y;
        char comma;
        ss >> x >> comma >> y;
        sharedGraph.addPoint(x, y);
        response = "Point added.";
    } else if (action == "Removepoint") {
        int x, y;
        char comma;
        ss >> x >> comma >> y;
        sharedGraph.removePoint(x, y);
        response = "Point removed.";
    } else if (action == "CH") {
        vector<Point*> hull = sharedGraph.getConvexHull();
        stringstream hullResponse;
        hullResponse << "Convex Hull Points: ";
        for (auto p : hull)
            hullResponse << *p << " ";
        response = hullResponse.str();
    } else {
        response = "Invalid command.";
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Binding
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    cout << "Server is listening on port " << PORT << endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        read(new_socket, buffer, BUFFER_SIZE);
        string command(buffer);
        string response;
        processCommand(command, response);

        send(new_socket, response.c_str(), response.length(), 0);
        close(new_socket);
    }

    return 0;
}
