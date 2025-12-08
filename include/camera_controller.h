#pragma once
#include "GameObject.h"  // For Vector3


class CameraController {
public:
    CameraController(int windowWidth, int windowHeight);

    void movePlayerAbsolute(float dx, float dy);
    void setPlayerPosition(float x, float y); // Set absolute position
    void updateRotation(int mouseX, int mouseY);
    void applyView();
    void getPlayerPosition(float* x, float* y, float* rotation) const;
    void reset();

    // Shooting support
    Vector3 getShootPosition() const;
    Vector3 getShootDirection() const;

private:
    float cameraDistance;
    float cameraAngle;
    float playerX;
    float playerY;
    float playerRotation;
    int windowWidth;
    int windowHeight;
};