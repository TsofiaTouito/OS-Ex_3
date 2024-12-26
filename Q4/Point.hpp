#include <iostream>




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
