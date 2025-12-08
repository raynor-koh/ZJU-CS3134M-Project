#pragma once

class Scene; // Forward declaration

class Camera {
public:
    Camera(float x = 0.0f, float y = 2.0f, float z = 10.0f);

    void applyView() const;
    void move(float forward, float right);
    void rotate(float deltaYaw, float deltaPitch);

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getZ() const { return z; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

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

    void updateVectors();
    float lookX, lookY, lookZ;  // Look direction
};
