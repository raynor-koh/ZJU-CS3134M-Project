#pragma once


class CameraController {
public:
    CameraController(int windowWidth, int windowHeight);

    void movePlayerAbsolute(float dx, float dy);
    void updateRotation(int mouseX, int mouseY);
    void applyView();
    void getPlayerPosition(float* x, float* y, float* rotation) const;
    void reset();

private:
    float cameraDistance;
    float cameraAngle;
    float playerX;
    float playerY;
    float playerRotation;
    int windowWidth;
    int windowHeight;
};