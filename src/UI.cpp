#include "ui.h"

UI::UI(int windowWidth, int windowHeight)
    : windowWidth(windowWidth), windowHeight(windowHeight),
      crossX(windowWidth / 2.0f), crossY(windowHeight / 2.0f), crossHalfSize(10.0f) {}

void UI::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void UI::setCrossPosition(float x, float y) {
    crossX = x;
    crossY = y;
}

void UI::drawCross() const {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // 横线
    glVertex2f(crossX - crossHalfSize, crossY);
    glVertex2f(crossX + crossHalfSize, crossY);
    // 竖线
    glVertex2f(crossX, crossY - crossHalfSize);
    glVertex2f(crossX, crossY + crossHalfSize);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
