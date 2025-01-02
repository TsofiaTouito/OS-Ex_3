#include "Point.hpp"

/**
 * @brief Determines the orientation of three points (this, mid, other).
 */
RelativeOrientation Point::orientation(const Point& mid, const Point& other) const {
    float orientationValue = (mid.getY() - getY()) * (other.getX() - getX()) -
                             (mid.getX() - getX()) * (other.getY() - getY());
    if (orientationValue == 0) return COLLINEAR;
    return (orientationValue > 0) ? CLOCKWISE : COUNTER_CLOCKWISE;
}
