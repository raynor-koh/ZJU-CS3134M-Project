#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>
#include <variant>
#include "Shapes.h"

struct Projection {
    float minValue, maxValue;
    
    Projection(float imin, float imax) : minValue(imin), maxValue(imax) {}
    
    bool overlaps(const Projection& other) const {
        return !(maxValue < other.minValue || other.maxValue < minValue);
    }
    
    static Projection merge(const Projection& a, const Projection& b) {
        return Projection(min(a.minValue, b.minValue), max(a.maxValue, b.maxValue));
    }
};

class CollisionDetector {
public:
    static bool checkCollision(Shape *shape1, Shape *shape2);
    static bool checkCollision(Cylinder& cyl1, Cylinder& cyl2);
    static bool checkCollision(Sphere& sph1, Sphere& sph2);
    static bool checkCollision(Cylinder& cyl, Sphere& sph);
    static bool checkCollision(Sphere& sph, Cube& cub);
    static bool checkCollision(Cylinder& cyl, Cube& cub);
    static bool checkCollision(Cube& cub1, Cube& cub2);
private:
    // Calculate Collision of Cylinder-Cylinder
    static constexpr float EPSILON = 1e-12f;
    static bool checkBoundingSphere(Cylinder& cyl1, Cylinder& cyl2);
    static bool checkSAT(Cylinder& cyl1, Cylinder& cyl2);
    static std::vector<Vector3> getSeparationAxes(Cylinder& cyl1, Cylinder& cyl2);
    static Projection projectCylinder(Cylinder& cyl, Vector3& axis);
    static bool checkParallelCylinder(Cylinder& cyl1, Cylinder& cyl2);
};

