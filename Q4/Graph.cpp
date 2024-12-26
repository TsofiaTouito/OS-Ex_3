#include "Graph.hpp"

using namespace std;



// Remove a point from the graph
void Graph::removePoint(int x, int y) {
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
void Graph::addPoint(int x, int y) {
    points.push_back(new Point(x, y));
}



// Function to find convex hull
vector<Point*> Graph::convexHull() {
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


// Angle comparison function for sorting
bool Graph::comparePolar(Point* p1, Point* p2, Point& origin) {
    float dx1 = p1->getX() - origin.getX(), dx2 = p2->getX() - origin.getX();
    float dy1 = p1->getY() - origin.getY(), dy2 = p2->getY() - origin.getY();
    return atan2(dy1, dx1) < atan2(dy2, dx2);
}
