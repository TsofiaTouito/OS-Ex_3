#include <complex>
#include "ConvexHull.hpp"

/**
 * @brief Calculates the area enclosed by the convex hull points using the shoelace formula.
 */
float ConvexHullUtility::computeEnclosedArea(vector<Point>& points) {
    size_t numPoints = points.size();
    float totalArea = 0.0;

    for (size_t i = 0; i < numPoints; ++i) {
        const Point& current = points[i];
        const Point& next = points[(i + 1) % numPoints]; // Wrap around to the first point
        totalArea += current.getX() * next.getY() - current.getY() * next.getX();
    }

    return std::abs(totalArea) / 2.0;
}

/**
 * @brief Computes the convex hull of a given set of points using Graham's scan algorithm.
 */
vector<Point> ConvexHullUtility::findConvexHull(vector<Point>& points) {
    size_t totalPoints = points.size(), hullIndex = 0;
    vector<Point> hull(totalPoints);

    // Sort points lexicographically
    std::sort(points.begin(), points.end());

    // Construct the lower hull
    for (size_t i = 0; i < totalPoints; ++i) {
        while (hullIndex >= 2 && hull[hullIndex - 2].orientation(hull[hullIndex - 1], points[i]) <= 0)
            hullIndex--;
        hull[hullIndex++] = points[i];
    }

    // Construct the upper hull
    for (size_t i = totalPoints - 1, startIdx = hullIndex + 1; i > 0; --i) {
        while (hullIndex >= startIdx && hull[hullIndex - 2].orientation(hull[hullIndex - 1], points[i - 1]) <= 0)
            hullIndex--;
        hull[hullIndex++] = points[i - 1];
    }

    hull.resize(hullIndex - 1); // Remove the last duplicate point
    return hull;
}
