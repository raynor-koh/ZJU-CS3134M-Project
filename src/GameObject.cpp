#include "GameObject.h"
#include <GL/glut.h>

GameObject::GameObject(const Vector3& position, const Vector3& size, const Color& color)
    : position(position), size(size), color(color), collisionType(CollisionType::SOLID) {
}

void GameObject::draw() const {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glColor3f(color.r, color.g, color.b);
    glScalef(size.x, size.y, size.z);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// AABB (Axis-Aligned Bounding Box) collision detection
// Checks if a circle (x, z, radius) collides with this box
bool GameObject::checkAABBCollision(float x, float z, float radius) const {
    if (collisionType == CollisionType::NONE) {
        return false; // No collision for non-solid objects
    }

    // Calculate box bounds (size is full dimensions, position is center)
    float boxMinX = position.x - size.x / 2.0f;
    float boxMaxX = position.x + size.x / 2.0f;
    float boxMinZ = position.z - size.z / 2.0f;
    float boxMaxZ = position.z + size.z / 2.0f;

    // Find closest point on box to circle center
    float closestX = (x < boxMinX) ? boxMinX : (x > boxMaxX) ? boxMaxX : x;
    float closestZ = (z < boxMinZ) ? boxMinZ : (z > boxMaxZ) ? boxMaxZ : z;

    // Calculate distance from circle center to closest point
    float distX = x - closestX;
    float distZ = z - closestZ;
    float distanceSquared = distX * distX + distZ * distZ;

    // Collision if distance is less than radius
    return distanceSquared < (radius * radius);
}
