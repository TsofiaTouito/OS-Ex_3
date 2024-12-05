#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <string>
#include <sstream>
#include <limits>

using namespace std;

//---------------------------------------------Graph & Point---------------------------------------------------------------- 

class Point {

private:
    int x;
    int y;

public:
    // Constructor
    Point(int x, int y) : x(x), y(y) {}

    int getX() const { return x; }
    int getY() const { return y; }

    // Overloading equality operator for comparison
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    // Overload << operator for printing
    friend std::ostream& operator<<(std::ostream& os, const Point& point) {
        os << "(" << point.x << ", " << point.y << ")";
        return os;
    }

};

class Graph {

public:
    vector<Point*> points;

    // Remove a point from the graph
    void removePoint(int x, int y) {
        auto it = std::remove_if(points.begin(), points.end(), [x, y](Point* point) {
            if (point->getX() == x && point->getY() == y) {
                delete point; // Free memory for the removed point
                return true;  // Remove from vector
            }
            return false;
        });
        points.erase(it, points.end());
    }

    // Adds a new point
    void add_point(float x, float y) {
        points.push_back(new Point(x, y));
    }

    // Destructor to free memory
    ~Graph() {
        for (Point* point : points) {
            delete point;
        }
    }
};

//------------------------------------------------------Convex hull-------------------------------------------------------

// Angle comparison function for sorting
bool comparePolar(Point* p1, Point* p2, Point& origin) {
    float dx1 = p1->getX() - origin.getX(), dx2 = p2->getX() - origin.getX();
    float dy1 = p1->getY() - origin.getY(), dy2 = p2->getY() - origin.getY();
    return atan2(dy1, dx1) < atan2(dy2, dx2);
}

// Function to find convex hull
vector<Point*> convexHull(vector<Point*>& points) {
    if (points.size() < 3) {
        return points;
    }

    // Finding the lowest point
    auto origin_iter = min_element(points.begin(), points.end(), [](Point* a, Point* b) {
        return (a->getY() < b->getY()) || (a->getY() == b->getY() && a->getX() < b->getX());
    });

    Point origin = **origin_iter;

    // Sort the points by angle
    sort(points.begin(), points.end(), [&origin](Point* a, Point* b) {
        return comparePolar(a, b, origin);
    });

    // Building the convex hull
    vector<Point*> hull;
    for (auto point : points) {
        while (hull.size() >= 2) {
            Point* p2 = hull.back();
            Point* p1 = hull[hull.size() - 2];

            float det = (p2->getX() - p1->getX()) * (point->getY() - p1->getY()) - (p2->getY() - p1->getY()) * (point->getX() - p1->getX());

            // Right turn
            if (det <= 0) {
                hull.pop_back();
            }
            // Left turn
            else {
                break;
            }
        }
        hull.push_back(point);
    }
    return hull;
}

//----------------------------------------------main program----------------------------------------------------


int main() {
    Graph graph;
    string command;

    cout << "Please enter a command:" << endl;

    while (true) {
        cout << "Enter command: ";
        getline(cin, command);  // Get the full line input

        if (command.empty()) {
            continue;  // Skip empty lines
        }

        if (command == "exit") {
            cout << "Exiting program." << endl;
            break;
        }

        if (command.find("Newgraph") == 0) {
            int n;
            cout << "Please enter the size n:" << endl;
            cin >> n;
            graph = Graph(); // Reset the graph

            cout << "Enter the points values in the format x, y:" << endl;
            for (int i = 0; i < n; i++) {
                float x, y;
                char comma;
                string line;
                getline(cin, line); // Read the full line of input
                stringstream ss(line); // Split the line using stringstream
                ss >> x >> comma >> y;
                graph.add_point(x, y);
            }
            cin.ignore();  // Ignore any remaining newline after number input
        }

        else if (command.find("CH") == 0) {
            vector<Point*> hull = convexHull(graph.points);

            cout << "Convex Hull:" << endl;
            for (auto p : hull) {
                cout << *p << endl;
            }
        }

        else if (command.find("NewPoint") == 0) {
            float x, y;
            char comma;
            cout << "Enter point coordinates x, y:" << endl;
            cin >> x >> comma >> y;
            graph.add_point(x, y);
            cin.ignore();  // Ignore any remaining newline after number input
        }

        else if (command.find("Removepoint") == 0) {
            float x, y;
            char comma;
            cout << "Enter point coordinates to remove (x, y):" << endl;
    
            if (cin >> x >> comma >> y) {
                graph.removePoint(x, y);
            } else {
                cout << "Invalid input! Please enter valid coordinates in the format x, y." << endl;
                cin.clear();  // Clear error flag
                cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Ignore invalid input
            }
        } else {
            cout << "Invalid input! Please enter a valid command." << endl;
        }
    }

    return 0;
}
