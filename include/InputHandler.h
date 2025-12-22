#pragma once
#include "Camera.h"
#include "camera_controller.h"
#include "Stob.h"
#include "UI.h"
#include "Player.h"
#include "Scene.h"

class Lighting; // Forward declaration

class InputHandler {
public:
    InputHandler(Camera* camera, CameraController* camera_controller, Scene* scene, Stob* controlledStob, Player* player, UI* gameUI, int windowWidth, int windowHeight);
    // 按键按下的处理
    void handleKeyPress(unsigned char key);
    void handleKeyRelease(unsigned char key);
    void handleSpecialKey(int key, int x, int y);  // For arrow keys, PageUp/PageDown
    void handleMouseClick(int button, int state, int x, int y);

    // 鼠标移动的处理
    void handleMouseMotion(int x, int y);

    // 更新方法，处理移动和摄像机操作
    void update();

    void setMouseSensitivity(float sensitivity) { mouseSensitivity = sensitivity; }
    void setWindowSize(int width, int height);
    bool isMouseCaptured() const { return mouseCaptured; }
    void toggleCamera();//改变摄像机：第三人称/第一人称

    // Lighting control
    void setLighting(Lighting* light) { lighting = light; }

private:
    void toggleMouseCapture();
    float lastTime = 0.0f;
    float mouseX, mouseY; // 当前鼠标位置（窗口坐标）
    Camera* camera = nullptr;                  // 第一人称摄像机
    CameraController* camera_controller = nullptr;  // 第三人称摄像机
    Scene* scene = nullptr;
    Stob* controlledStob = nullptr;        // 被控制的角色对象
    Player* player = nullptr;
    UI* gameUI = nullptr;                      // 游戏UI对象
    bool keys[256];
    int windowWidth, windowHeight;
    int lastMouseX, lastMouseY;
    bool firstMouse;
    bool mouseCaptured;
    float mouseSensitivity;

    Lighting* lighting;  // Not owned by InputHandler, just a reference
};
