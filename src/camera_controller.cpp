#define _USE_MATH_DEFINES
#include "camera_controller.h"
#include <GL/glut.h>
#include <cmath>
CameraController::CameraController(int windowWidth, int windowHeight)
    : cameraDistance(10.0f), cameraAngle(0.0f), playerX(0.0f), playerY(0.0f), playerRotation(0.0f),
      windowWidth(windowWidth), windowHeight(windowHeight) {}

void CameraController::movePlayerAbsolute(float dx, float dy) {
    playerX += dx;
    playerY += dy;
}

void CameraController::updateRotation(int mouseX, int mouseY) {
    float centerX = windowWidth / 2.0f;
    float centerY = windowHeight / 2.0f;
    float deltaX = mouseX - centerX;
    float deltaY = centerY - mouseY;

    float angle = atan2f(deltaY, deltaX) * 180.0f / M_PI;
    playerRotation = angle - 90.0f;
}

void CameraController::applyView() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)windowWidth / windowHeight, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(playerX, cameraDistance, playerY, playerX, 0.0f, playerY, 0.0f, 0.0f, 1.0f);
}

void CameraController::getPlayerPosition(float* x, float* y, float* rotation) const {
    if (x) *x = playerX;
    if (y) *y = playerY;
    if (rotation) *rotation = playerRotation;
}

void CameraController::reset() {
    playerX = 0.0f;
    playerY = 0.0f;
    playerRotation = 0.0f;
}