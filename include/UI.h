#pragma once
#include <GL/glut.h>

class UI {
public:
    UI(int windowWidth, int windowHeight);

    void setWindowSize(int width, int height);

    // 设置准星在屏幕空间中的位置（像素坐标）
    void setCrossPosition(float x, float y);

    // 绘制准星（在 display 中 3D 场景绘制完之后调用）
    void drawCross() const;

private:
    int windowWidth;
    int windowHeight;
    float crossX;
    float crossY;
    float crossHalfSize;
};
