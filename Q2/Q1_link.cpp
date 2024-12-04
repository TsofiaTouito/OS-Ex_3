#include <iostream>
#include <list>
#include <memory>
#include <algorithm>
#include <cmath>
#include <cstdlib> 
#include <ctime>

using namespace std;

struct Point {
    float* x;
    float* y;

public:
    // Constructor
    Point(float x, float y) : x(new float(x)), y(new float(y)) {}

    // Copy constructor
    Point(const Point& other) : x(new float(*other.x)), y(new float(*other.y)) {}

    // Move constructor
    Point(Point&& other) noexcept : x(other.x), y(other.y) {
        other.x = nullptr;
        other.y = nullptr;
    }

    // Copy assignment operator
    Point& operator=(const Point& other) {
        if (this != &other) {
            *x = *other.x;
            *y = *other.y;
        }
        return *this;
    }

    // Move assignment operator
    Point& operator=(Point&& other) noexcept {
        if (this != &other) {
            delete x;
            delete y;

            x = other.x;
            y = other.y;

            other.x = nullptr;
            other.y = nullptr;
        }
        return *this;
    }

    float getX() const { return *x; }
    float getY() const { return *y; }

    ~Point() {
        delete x;
        delete y;
    }
};

// Angle comparison function for sorting
bool comparePolar(Point* p1, Point* p2, Point& origin) {
    float dx1 = p1->getX() - origin.getX(), dx2 = p2->getX() - origin.getX();
    float dy1 = p1->getY() - origin.getY(), dy2 = p2->getY() - origin.getY();
    return atan2(dy1, dx1) < atan2(dy2, dx2);
}

// Function to find convex hull
list<Point*> convexHull(list<Point*>& points) {
    if (points.size() < 3) {
        return points;
    }

    // Finding the lowest point
    auto origin_iter = min_element(points.begin(), points.end(), [](Point* a, Point* b) {
        return (a->getY() < b->getY()) || (a->getY() == b->getY() && a->getX() < b->getX());
    });

    Point origin = **origin_iter;

    // Sort the points by angle
    points.sort([&origin](Point* a, Point* b) {
        return comparePolar(a, b, origin);
    });

    // Building the convex hull
    list<Point*> hull;
    for (auto point : points) {
        while (hull.size() >= 2) {
            auto p2 = hull.back();
            auto p1 = *next(hull.rbegin(), 1);  // Accessing second-to-last element

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



// Function to calculate polygon area
float polygonArea(list<Point*>& polygon) {
    float area = 0.0;

    // Iterate through the list using an iterator
    auto it = polygon.begin();
    auto nextIt = std::next(it);

    while (nextIt != polygon.end()) {
        Point* curr = *it;
        Point* next = *nextIt;

        area += curr->getX() * next->getY() - next->getX() * curr->getY();

        ++it;
        ++nextIt;
    }

    // To complete the polygon by closing the loop (last point to the first point)
    if (polygon.size() > 1) {
        Point* first = polygon.front();
        Point* last = polygon.back();
        area += last->getX() * first->getY() - first->getX() * last->getY();
    }

    return abs(area) / 2.0;
}



int main() {

    srand(time(0));

    cout << "Please enter the number of points: " << endl;
    int num;
    cin >> num;

    list<Point*> points;
    cout << "Generating points..." << endl;

    // create points values
    for (int i = 0; i < num; i++) {
        float x = rand() % 100;
        float y = rand() % 100;
        points.push_back(new Point(x, y));
    }

    list<Point*> hull = convexHull(points);
    float area = polygonArea(hull);

    cout << "The area is: " << area << endl;

    // Cleanup memory
    for (auto point : points) {
        delete point;
    }

    return 0;
}
