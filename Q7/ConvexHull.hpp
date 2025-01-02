#include <vector>
#include <algorithm>
#include <stack>
#include "Point.hpp"

#ifndef CONVEXHULL_HPP
#define CONVEXHULL_HPP

using std::vector;

/**
 * @brief A utility class for computing the convex hull of a set of points
 * and calculating the area of the convex hull.
 */
class ConvexHullUtility {
private:
    /**
     * @brief Calculates the area enclosed by the given points.
     * 
     * @param points A vector of points representing the convex hull.
     * @return The area of the convex hull.
     */
    static float computeEnclosedArea(vector<Point>& points);

public:
    /**
     * @brief Computes the convex hull of a given set of points.
     * 
     * @param points A vector of points.
     * @return A vector of points representing the convex hull.
     */
    static vector<Point> findConvexHull(vector<Point>& points);

    /**
     * @brief Computes the area of the convex hull for a given set of points.
     * 
     * @param points A vector of points.
     * @return The area of the convex hull.
     */
    static float computeHullArea(vector<Point>& points) {
        vector<Point> hullPoints = findConvexHull(points);
        return computeEnclosedArea(hullPoints);
    }
};

#endif // CONVEXHULL_HPP
