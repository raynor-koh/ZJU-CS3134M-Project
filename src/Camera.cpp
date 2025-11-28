#define _USE_MATH_DEFINES
#include "Camera.h"
#include <GL/glut.h>
#include <cmath>

Camera::Camera(float x, float y, float z)
    : x(x), y(y), z(z), yaw(0.0f), pitch(0.0f), moveSpeed(0.2f) {
    updateVectors();
}

void Camera::updateVectors() {
    lookX = x + cos(pitch * M_PI / 180.0f) * sin(yaw * M_PI / 180.0f);
    lookY = y + sin(pitch * M_PI / 180.0f);
    lookZ = z - cos(pitch * M_PI / 180.0f) * cos(yaw * M_PI / 180.0f);
}

void Camera::applyView() const {
    gluLookAt(x, y, z,           // Camera position
              lookX, lookY, lookZ, // Look at point
              0.0f, 1.0f, 0.0f);  // Up vector
}

void Camera::move(float forward, float right) {
    x += forward * sin(yaw * M_PI / 180.0f) * moveSpeed;
    z -= forward * cos(yaw * M_PI / 180.0f) * moveSpeed;

    x += right * cos(yaw * M_PI / 180.0f) * moveSpeed;
    z += right * sin(yaw * M_PI / 180.0f) * moveSpeed;

    updateVectors();
}

void Camera::rotate(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    pitch += deltaPitch;

    // Clamp pitch to prevent flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateVectors();
}

void Camera::setPosition(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    updateVectors();
}
