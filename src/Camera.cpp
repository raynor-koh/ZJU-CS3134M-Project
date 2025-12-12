#define _USE_MATH_DEFINES
#include "Camera.h"
#include "Scene.h"
#include <GL/glut.h>
#include <cmath>
//第一人称摄像头
Camera::Camera(float x, float y, float z)
    : x(x), y(y), z(z), yaw(0.0f), pitch(0.0f), moveSpeed(0.2f),
      collisionRadius(0.5f), scene(nullptr), verticalVelocity(0.0f),
      groundLevel(1.5f), isOnGround(true) {
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
    // Calculate new position
    float newX = x + forward * sin(yaw * M_PI / 180.0f) * moveSpeed;
    float newZ = z - forward * cos(yaw * M_PI / 180.0f) * moveSpeed;

    newX += right * cos(yaw * M_PI / 180.0f) * moveSpeed;
    newZ += right * sin(yaw * M_PI / 180.0f) * moveSpeed;

    // Check collision before applying movement
    if (scene == nullptr || !scene->checkCollision(newX, newZ, collisionRadius)) {
        x = newX;
        z = newZ;
    }

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

void Camera::update(float deltaTime) {
    // Apply gravity
    if (!isOnGround) {
        verticalVelocity -= GRAVITY * deltaTime;
    }

    // Update Y position based on vertical velocity
    y += verticalVelocity * deltaTime;

    // Ground collision
    if (y <= groundLevel) {
        y = groundLevel;
        verticalVelocity = 0.0f;
        isOnGround = true;
    } else {
        isOnGround = false;
    }

    updateVectors();
}

void Camera::jump() {
    if (isOnGround) {
        verticalVelocity = JUMP_VELOCITY;
        isOnGround = false;
    }
}
