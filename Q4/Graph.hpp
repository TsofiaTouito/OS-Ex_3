#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>
#include <string>
#include <sstream>
#include <limits>
#include "Point.hpp"

using namespace std;

class Graph {

    public:
    vector<Point*> points;

    void removePoint(int x, int y); // Remove a point from the graph
    void addPoint(int x, int y);   // Adds a new point
    vector<Point*> convexHull();    // Function to find convex hull


     // Destructor to free memory
    ~Graph() {
        for (Point* point : points) {
            delete point;
        }
    }

    private:
    // Angle comparison function for sorting
    static bool comparePolar(Point* p1, Point* p2, Point& origin) ;


};