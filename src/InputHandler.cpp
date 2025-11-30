#include "InputHandler.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

extern bool Active_Third_Camera; // 声明外部变量
InputHandler::InputHandler(Camera* camera, CameraController* camera_controller, int windowWidth, int windowHeight)
    : camera(camera), camera_controller(camera_controller), windowWidth(windowWidth), windowHeight(windowHeight),
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
    else if (key == 'c' || key == 'C') { // 'C' 键 - 切换摄像机
        toggleCamera();
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

    if (Active_Third_Camera == false){ 
        // 第一人称摄像机旋转
        camera->rotate(deltaX * mouseSensitivity, -deltaY * mouseSensitivity);
        // Re-center mouse cursor
        if (abs(x - windowWidth / 2) > 100 || abs(y - windowHeight / 2) > 100) {
            glutWarpPointer(windowWidth / 2, windowHeight / 2);
            lastMouseX = windowWidth / 2;
            lastMouseY = windowHeight / 2;
        }
        
    }
    else if (Active_Third_Camera == true){
        //第三人称，角色转向变化
        camera_controller->updateRotation(x, y); 
    }

    
}

void InputHandler::update() {
    if(Active_Third_Camera == false){ 
        // 第一人称 wasd控制 摄像机移动
        
        float forward = 0.0f, right = 0.0f;

        if (keys['w'] || keys['W']) forward += 1.0f;
        if (keys['s'] || keys['S']) forward -= 1.0f;
        if (keys['a'] || keys['A']) right -= 1.0f;
        if (keys['d'] || keys['D']) right += 1.0f;

        if (forward != 0.0f || right != 0.0f) {
            camera->move(forward, right);
        }
    }
    else if (Active_Third_Camera == true){
        //第三人称wasd控制角色移动
    // 计算时间增量
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // 转换为秒
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

	float moveX = 0, moveY = 0;

    float moveSpeed = 5.0f; // 调整为合适的速度值
	// 水平移动
	if(keys['a']||keys['A']) moveX += 0.1f;
	if(keys['d']||keys['D']) moveX -= 0.1f;
	
	// 垂直移动  
	if(keys['s']||keys['S']) moveY -= 0.1f;
	if(keys['w']||keys['W']) moveY += 0.1f;
	
	// 标准化所有组合移动的速度（包括相邻键组合）
	float length = sqrt(moveX * moveX + moveY * moveY);
	if(length > 0.001f) {
        moveX = moveX / length * moveSpeed * deltaTime;
        moveY = moveY / length * moveSpeed * deltaTime;
        
        camera_controller->movePlayerAbsolute(moveX, moveY);
        glutPostRedisplay();
    }
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
void InputHandler::toggleCamera() {
    Active_Third_Camera = !Active_Third_Camera;
}
