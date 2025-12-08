#pragma once
#include "GameObject.h"  // For Vector3

class Scene; // Forward declaration

class Camera {
public:
    Camera(float x = 0.0f, float y = 2.0f, float z = 10.0f);

    void applyView() const;
    void move(float forward, float right);
    void rotate(float deltaYaw, float deltaPitch);
    void update(float deltaTime);  // Update physics (gravity, jumping)
    void jump();  // Trigger jump

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
    Vector3 getLookDirection() const { return Vector3(lookX - x, lookY - y, lookZ - z); }

    // Setters
    void setPosition(float x, float y, float z);
    void setMoveSpeed(float speed) { moveSpeed = speed; }
    float getMoveSpeed() const { return moveSpeed; }

    // Collision
    void setScene(Scene* scene) { this->scene = scene; }
    void setCollisionRadius(float radius) { collisionRadius = radius; }
    float getCollisionRadius() const { return collisionRadius; }

private:
    float x, y, z;          // Position
    float yaw, pitch;       // Rotation angles
    float moveSpeed;        // Movement speed
    float collisionRadius;  // Collision detection radius
    Scene* scene;           // Scene reference for collision detection

    // Jump physics
    float verticalVelocity; // Vertical velocity for jumping
    float groundLevel;      // Y position of the ground
    bool isOnGround;        // Whether camera is on the ground
    static constexpr float GRAVITY = 20.0f;      // Gravity acceleration
    static constexpr float JUMP_VELOCITY = 8.0f; // Initial jump velocity

    void updateVectors();
    float lookX, lookY, lookZ;  // Look direction
};
