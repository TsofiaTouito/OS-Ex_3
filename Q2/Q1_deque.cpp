#include <iostream>
#include <deque>
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
deque<Point*> convexHull(deque<Point*>& points) {
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
    deque<Point*> hull;
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

// Function to calculate polygon area
float polygonArea(deque<Point*>& polygon) {
    float area = 0.0;
    int n = polygon.size();

    for (int i = 0; i < n; i++) {
        Point* curr = polygon[i];
        Point* next = polygon[(i + 1) % n];

        area += curr->getX() * next->getY() - next->getX() * curr->getY();
    }

    return abs(area) / 2.0;
}

int main() {

    srand(time(0));

    cout << "Please enter the number of points: " << endl;
    int num;
    cin >> num;

    deque<Point*> points;
    cout << "Generating points..." << endl;

    // create points values
    for (int i = 0; i < num; i++) {
        float x = rand() % 100;
        float y = rand() % 100;
        points.push_back(new Point(x, y));
    }

    deque<Point*> hull = convexHull(points);
    float area = polygonArea(hull);

    cout << "The area is: " << area << endl;

    // Cleanup memory
    for (auto point : points) {
        delete point;
    }

    return 0;
}
