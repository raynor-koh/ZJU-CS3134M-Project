#pragma once
#include "Camera.h"
#include "camera_controller.h"
class InputHandler {
public:
    InputHandler(Camera* camera, CameraController* camera_controller, int windowWidth, int windowHeight);
    // 按键按下的处理
    void handleKeyPress(unsigned char key);
    void handleKeyRelease(unsigned char key);

    // 鼠标移动的处理
    void handleMouseMotion(int x, int y);

    // 更新方法，处理移动和摄像机操作
    void update();

    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setWindowSize(int width, int height);
    bool isMouseCaptured() const { return mouseCaptured; }
    void toggleCamera();//改变摄像机：第三人称/第一人称
private:
    void toggleMouseCapture();
    float lastTime = 0.0f;
    
    Camera* camera = nullptr;                  // 第一人称摄像机
    CameraController* camera_controller = nullptr;  // 第三人称摄像机
    

    bool keys[256];
    int windowWidth, windowHeight;
    int lastMouseX, lastMouseY;
    bool firstMouse;
    bool mouseCaptured;
    float mouseSensitivity;
};
