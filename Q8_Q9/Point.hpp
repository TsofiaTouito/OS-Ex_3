#include <cmath>

#ifndef GEOMETRY_UTILS_POINT_HPP
#define GEOMETRY_UTILS_POINT_HPP

/**
 * @brief Enumeration representing the orientation of three points.
 */
enum RelativeOrientation {
    COLLINEAR = 0,
    CLOCKWISE = -1,
    COUNTER_CLOCKWISE = 1
};

/**
 * @brief A class representing a 2D point with basic geometric operations.
 */
class Point {
private:
    float coordinateX;
    float coordinateY;

public:
    Point() : coordinateX(0), coordinateY(0) {}
    Point(float x, float y) : coordinateX(x), coordinateY(y) {}

    float getX() const { return coordinateX; }
    float getY() const { return coordinateY; }

    /**
     * @brief Computes the orientation of three points (this, mid, other).
     * 
     * @param mid The second point.
     * @param other The third point.
     * @return The orientation: COLLINEAR, CLOCKWISE, or COUNTER_CLOCKWISE.
     */
    RelativeOrientation orientation(const Point& mid, const Point& other) const;

    /**
     * @brief Calculates the Euclidean distance between this point and another point.
     * 
     * @param other The other point.
     * @return The distance between the points.
     */
    double computeDistance(const Point& other) const {
        return sqrt(pow(coordinateX - other.coordinateX, 2) + pow(coordinateY - other.coordinateY, 2));
    }

    bool operator==(const Point& other) const {
        return coordinateX == other.coordinateX && coordinateY == other.coordinateY;
    }
    bool operator!=(const Point& other) const { return !(*this == other); }

    bool operator<(const Point& other) const {
        return coordinateX < other.coordinateX || (coordinateX == other.coordinateX && coordinateY < other.coordinateY);
    }
    bool operator>(const Point& other) const { return other < *this; }
    bool operator<=(const Point& other) const { return !(*this > other); }
    bool operator>=(const Point& other) const { return !(*this < other); }
};

#endif // GEOMETRY_UTILS_POINT_HPP
