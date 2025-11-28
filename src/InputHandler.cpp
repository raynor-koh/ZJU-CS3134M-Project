#include "InputHandler.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

InputHandler::InputHandler(Camera* camera, int windowWidth, int windowHeight)
    : camera(camera), windowWidth(windowWidth), windowHeight(windowHeight),
      firstMouse(true), mouseCaptured(true), mouseSensitivity(0.1f) {

    for (int i = 0; i < 256; i++) {
        keys[i] = false;
    }

    lastMouseX = windowWidth / 2;
    lastMouseY = windowHeight / 2;
}

void InputHandler::handleKeyPress(unsigned char key) {
    keys[key] = true;

    if (key == 9) { // Tab key - toggle mouse capture
        toggleMouseCapture();
    }
    else if (key == 27) { // ESC key
        exit(0);
    }
}

void InputHandler::handleKeyRelease(unsigned char key) {
    keys[key] = false;
}

void InputHandler::handleMouseMotion(int x, int y) {
    if (!mouseCaptured) {
        return; // Don't process mouse movement when not captured
    }

    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
    }

    float deltaX = x - lastMouseX;
    float deltaY = y - lastMouseY;

    lastMouseX = x;
    lastMouseY = y;

    camera->rotate(deltaX * mouseSensitivity, -deltaY * mouseSensitivity);

    // Re-center mouse cursor
    if (abs(x - windowWidth / 2) > 100 || abs(y - windowHeight / 2) > 100) {
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
        lastMouseX = windowWidth / 2;
        lastMouseY = windowHeight / 2;
    }
}

void InputHandler::update() {
    float forward = 0.0f, right = 0.0f;

    if (keys['w'] || keys['W']) forward += 1.0f;
    if (keys['s'] || keys['S']) forward -= 1.0f;
    if (keys['a'] || keys['A']) right -= 1.0f;
    if (keys['d'] || keys['D']) right += 1.0f;

    if (forward != 0.0f || right != 0.0f) {
        camera->move(forward, right);
    }
}

void InputHandler::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void InputHandler::toggleMouseCapture() {
    mouseCaptured = !mouseCaptured;
    firstMouse = true; // Reset to avoid camera jump

    if (mouseCaptured) {
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
    } else {
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }
}
