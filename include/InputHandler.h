#pragma once
#include "Camera.h"

class InputHandler {
public:
    InputHandler(Camera* camera, int windowWidth, int windowHeight);

    void handleKeyPress(unsigned char key);
    void handleKeyRelease(unsigned char key);
    void handleMouseMotion(int x, int y);
    void update();

    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setWindowSize(int width, int height);
    bool isMouseCaptured() const { return mouseCaptured; }

private:
    void toggleMouseCapture();

    Camera* camera;
    bool keys[256];
    int windowWidth, windowHeight;
    int lastMouseX, lastMouseY;
    bool firstMouse;
    bool mouseCaptured;
    float mouseSensitivity;
};
