#pragma once

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

private:
    float x, y, z;          // Position
    float yaw, pitch;       // Rotation angles
    float moveSpeed;        // Movement speed

    void updateVectors();
    float lookX, lookY, lookZ;  // Look direction
};
