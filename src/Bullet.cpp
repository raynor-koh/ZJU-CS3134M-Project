#include "Bullet.h"
#include <GL/glut.h>
#include <cmath>

Bullet::Bullet(const Vector3& position, const Vector3& direction, float speed, float damage)
    : position(position), speed(speed), radius(0.1f), damage(damage), lifetime(0.0f), maxLifetime(5.0f), active(true) {

    // Normalize direction vector
    float length = std::sqrt(direction.x * direction.x +
                            direction.y * direction.y +
                            direction.z * direction.z);

    if (length > 0.0f) {
        velocity.x = (direction.x / length) * speed;
        velocity.y = (direction.y / length) * speed;
        velocity.z = (direction.z / length) * speed;
    } else {
        // Default direction if invalid
        velocity.x = 0.0f;
        velocity.y = 0.0f;
        velocity.z = -speed;
    }
}

void Bullet::update(float deltaTime) {
    if (!active) return;

    // Update position based on velocity
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
    position.z += velocity.z * deltaTime;

    // Update lifetime
    lifetime += deltaTime;
}

void Bullet::draw() const {
    if (!active) return;

    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);

    // Yellow bullet color
    glColor3f(1.0f, 1.0f, 0.0f);

    glutSolidSphere(radius, 8, 8);
    glPopMatrix();
}

bool Bullet::shouldRemove() const {
    return !active || lifetime >= maxLifetime;
}
