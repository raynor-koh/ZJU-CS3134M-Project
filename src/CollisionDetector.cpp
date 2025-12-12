#include "CollisionDetector.h"

bool CollisionDetector::checkCollision(Shape *shape1, Shape *shape2) {
    Shape::ShapeType type1 = shape1->getType();
    Shape::ShapeType type2 = shape2->getType();
    if(type1 == Shape::CYLINDER && type2 == Shape::CYLINDER) {
        Cylinder* cyl1 = (Cylinder*)shape1;
        Cylinder* cyl2 = (Cylinder*)shape2;
        return checkCollision(*cyl1, *cyl2);
    }
    else if(type1 == Shape::CYLINDER && type2 == Shape::SPHERE) {
        Cylinder* cyl = (Cylinder*)shape1;
        Sphere* sph = (Sphere*)shape2;
        return checkCollision(*cyl, *sph);
    }
    else if(type1 == Shape::SPHERE && type2 == Shape::SPHERE) {
        Sphere* sph1 = (Sphere*)shape1;
        Sphere* sph2 = (Sphere*)shape2;
        return checkCollision(*sph1, *sph2);
    }
    else if(type1 == Shape::SPHERE && type2 == Shape::CYLINDER) {
        Cylinder* cyl = (Cylinder*)shape2;
        Sphere* sph = (Sphere*)shape1;
        return checkCollision(*cyl, *sph);
    }
    else if(type1 == Shape::CYLINDER && type2 == Shape::CUBE) {
        Cylinder* cyl = (Cylinder*)shape1;
        Cube* cub = (Cube*)shape2;
        return checkCollision(*cyl, *cub);
    }
    else if(type1 == Shape::CUBE && type2 == Shape::CYLINDER) {
        Cylinder* cyl = (Cylinder*)shape2;
        Cube* cub = (Cube*)shape1;
        return checkCollision(*cyl, *cub);
    }
    else if(type1 == Shape::CUBE && type2 == Shape::SPHERE) {
        Sphere* sph = (Sphere*)shape2;
        Cube* cub = (Cube*)shape1;
        return checkCollision(*sph, *cub);
    }
    else if(type1 == Shape::SPHERE && type2 == Shape::CUBE) {
        Sphere* sph = (Sphere*)shape1;
        Cube* cub = (Cube*)shape2;
        return checkCollision(*sph, *cub);
    }
    else if(type1 == Shape::CUBE && type2 == Shape::CUBE) {
        Cube* cub1 = (Cube*)shape1;
        Cube* cub2 = (Cube*)shape2;
        return checkCollision(*cub1, *cub2);
    }
    else return false;
}

// Collision of Cylinder-Cylinder
bool CollisionDetector::checkCollision(Cylinder& cyl1, Cylinder& cyl2) {
    if(!checkBoundingSphere(cyl1, cyl2)) return false;
    float dot = std::abs(cyl1.getAxis().dot(cyl2.getAxis()));
    if (std::abs(dot - 1.0f) < 1e-6f) {
        return checkParallelCylinder(cyl1, cyl2);
    }
    return checkSAT(cyl1, cyl2);
}

bool CollisionDetector::checkBoundingSphere(Cylinder& cyl1, Cylinder& cyl2) { 
    float distance = (cyl1.getPosition() - cyl2.getPosition()).length();
    float bound1 = cyl1.getBoundingSphereRadius();
    float bound2 = cyl2.getBoundingSphereRadius();
    return distance <= bound1 + bound2;
}

bool CollisionDetector::checkSAT(Cylinder& cyl1, Cylinder& cyl2) {
    std::vector<Vector3> axes = getSeparationAxes(cyl1, cyl2);
    for(const Vector3& axis : axes) {
        if(axis.length() < EPSILON) continue;
        Vector3 unitAxis = axis.normalized();
        Projection proj1 = projectCylinder(cyl1, unitAxis);
        Projection proj2 = projectCylinder(cyl2, unitAxis);
        if(!proj1.overlaps(proj2)) return false;
    }
    return true;
}

std::vector<Vector3> CollisionDetector::getSeparationAxes(Cylinder& cyl1, Cylinder& cyl2) { 
    std::vector<Vector3> axes;
    axes.push_back(cyl1.getAxis());
    axes.push_back(cyl2.getAxis());

    Vector3 crossAxis = cyl1.getAxis().cross(cyl2.getAxis());
    if(crossAxis.length() > EPSILON) axes.push_back(crossAxis);

    Vector3 v = cyl2.getPosition() - cyl1.getPosition();
    if(v.length() > EPSILON) {
        Vector3 axis1 = cyl1.getAxis().cross(v);
        if(axis1.length() > EPSILON) axes.push_back(axis1);
        Vector3 axis2 = cyl2.getAxis().cross(v);
        if(axis2.length() > EPSILON) axes.push_back(axis2);
    }
    return axes;
}

Projection CollisionDetector::projectCylinder(Cylinder& cyl, Vector3& axis) {
        float centerProj = cyl.getPosition().dot(axis);
        
        float axisProj = cyl.getAxis().dot(axis);
        
        float heightContribution = (cyl.getSize().y / 2.0f) * std::abs(axisProj);
        
        float radiusContribution = cyl.getSize().x / 2.0f * std::sqrt(1.0f - axisProj * axisProj);
        
        if (std::isnan(radiusContribution)) {
            radiusContribution = 0.0;
        }
        
        float total = heightContribution + radiusContribution;
        return Projection(centerProj - total, centerProj + total);
}

bool CollisionDetector::checkParallelCylinder(Cylinder& cyl1, Cylinder& cyl2) {
    Vector3 diff = cyl2.getPosition() - cyl1.getPosition();
    Vector3 axis = cyl1.getAxis();
    bool axisOverlap = abs(diff.dot(axis)) <= cyl1.getSize().y / 2.0f + cyl2.getSize().y / 2.0f;
    bool perpOverlap = diff.cross(axis).length() <= cyl1.getSize().x / 2.0f + cyl2.getSize().x / 2.0f;
    return axisOverlap && perpOverlap;
}

// Collision of Sphere-Sphere
bool CollisionDetector::checkCollision(Sphere& sph1, Sphere& sph2) { 
    Vector3 diff = sph2.getPosition() - sph1.getPosition();
    float distance = diff.length();
    float radiusSum = sph1.getRadius() + sph2.getRadius();
    return distance <= radiusSum;
}

// Collision of Cylinder-Sphere
bool CollisionDetector::checkCollision(Cylinder& cyl, Sphere& sph) {
    Vector3 diff = sph.getPosition() - cyl.getPosition();
    Vector3 axis = cyl.getAxis();
    float axialDist = std::abs(diff.dot(axis));
    float normDist = diff.cross(axis).length();
    if(normDist > cyl.getSize().x / 2.0f + sph.getRadius()) return false;
    if(axialDist > cyl.getSize().y / 2.0f + sph.getRadius()) return false;
    if((axialDist <= cyl.getSize().y / 2.0f + sph.getRadius()) && normDist <= cyl.getSize().x / 2.0f) return true;
    if(normDist <= cyl.getSize().x / 2.0f) return true;
    Vector3 vertTemp = diff.cross(axis);
    Vector3 normTemp = axis.cross(vertTemp).normalized() * cyl.getSize().x / 2.0f;
    Vector3 axialTemp = axis * cyl.getSize().y / 2.0f;
    if((axialTemp - normTemp - diff).length() <= sph.getRadius()) return true;
    if((axialTemp + normTemp - diff).length() <= sph.getRadius()) return true;
    if((axialTemp - normTemp + diff).length() <= sph.getRadius()) return true;
    if((axialTemp + normTemp + diff).length() <= sph.getRadius()) return true;
    return false;
}

// Collision of Cube-Sphere
bool CollisionDetector::checkCollision(Sphere& sph, Cube& cub) {
    Vector3 diff = sph.getPosition() - cub.getPosition();
    Vector3 minDiff = sph.getSize() / 2.0f + cub.getSize();
    if(diff.x < -minDiff.x || diff.x > minDiff.x) return false;
    if(diff.y < -minDiff.y || diff.y > minDiff.y) return false;
    if(diff.z < -minDiff.z || diff.z > minDiff.z) return false;
    return true;
}

bool CollisionDetector::checkCollision(Cylinder& cyl, Cube& cub) {
    Vector3 diff = cub.getPosition() - cyl.getPosition();
    Vector3 minDiff = cyl.getSize() / 2.0f + cub.getSize();
    if(diff.x < -minDiff.x || diff.x > minDiff.x) return false;
    if(diff.y < -minDiff.y || diff.y > minDiff.y) return false;
    if(diff.z < -minDiff.z || diff.z > minDiff.z) return false;
    return true;
}

bool CollisionDetector::checkCollision(Cube& cub1, Cube& cub2) {
    Vector3 diff = cub2.getPosition() - cub1.getPosition();
    Vector3 minDiff = cub1.getSize() + cub2.getSize();
    return !(diff.x < -minDiff.x || diff.x > minDiff.x ||
             diff.y < -minDiff.y || diff.y > minDiff.y ||
             diff.z < -minDiff.z || diff.z > minDiff.z);
}