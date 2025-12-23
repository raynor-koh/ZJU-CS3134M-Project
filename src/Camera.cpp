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
    const float PLAYER_HEIGHT = 1.5f; // Match the height used in update()

    // Calculate new position
    float newX = x + forward * sin(yaw * M_PI / 180.0f) * moveSpeed;
    float newZ = z - forward * cos(yaw * M_PI / 180.0f) * moveSpeed;

    newX += right * cos(yaw * M_PI / 180.0f) * moveSpeed;
    newZ += right * sin(yaw * M_PI / 180.0f) * moveSpeed;

    // Check collision before applying movement (Y-aware)
    // Player's feet are at y - PLAYER_HEIGHT
    float playerBottom = y - PLAYER_HEIGHT;
    if (scene == nullptr || !scene->checkCollision(newX, playerBottom, newZ, collisionRadius, PLAYER_HEIGHT)) {
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
    const float SKIN_WIDTH = 0.01f; // Small epsilon for stable landing
    const float PLAYER_HEIGHT = 1.5f; // Player capsule height

    // Apply gravity
    if (!isOnGround) {
        verticalVelocity -= GRAVITY * deltaTime;
    }

    // Store old position
    float oldY = y;

    // Update Y position based on vertical velocity (axis-separated: Y first)
    y += verticalVelocity * deltaTime;

    // Check for vertical collision with scene objects
    float groundY = 0.0f; // Default ground level
    bool foundGround = false;

    if (scene) {
        // Calculate player bottom position (y is eye level, subtract to get feet)
        float playerBottom = y - PLAYER_HEIGHT;

        // Check vertical collision with boxes
        scene->checkVerticalCollision(x, playerBottom, z, collisionRadius, PLAYER_HEIGHT, groundY);

        // Landing detection: if moving downward and we're at or below a surface
        if (verticalVelocity <= 0.0f && playerBottom <= groundY + SKIN_WIDTH) {
            // Snap to surface with skin width for stable landing
            y = groundY + PLAYER_HEIGHT + SKIN_WIDTH;
            verticalVelocity = 0.0f;
            isOnGround = true;
            foundGround = true;
        }
        // Ceiling collision: if moving upward and hit ceiling
        else if (verticalVelocity > 0.0f) {
            // Check if head hit something (only trigger if actually found a box)
            float ceilingY = 0.0f;
            if (scene->checkVerticalCollision(x, y, z, collisionRadius, 0.1f, ceilingY) && ceilingY > 0.0f) {
                if (y >= ceilingY - SKIN_WIDTH) {
                    // Hit ceiling, stop upward movement
                    verticalVelocity = 0.0f;
                    y = oldY; // Revert to previous position
                }
            }
        }
    }

    // Fallback to default ground level if no box found
    if (!foundGround) {
        float playerBottom = y - PLAYER_HEIGHT;
        if (playerBottom <= 0.0f + SKIN_WIDTH) {
            y = PLAYER_HEIGHT + SKIN_WIDTH;
            verticalVelocity = 0.0f;
            isOnGround = true;
        } else {
            isOnGround = false;
        }
    }

    updateVectors();
}

void Camera::jump() {
    if (isOnGround) {
        verticalVelocity = JUMP_VELOCITY;
        isOnGround = false;
    }
}
