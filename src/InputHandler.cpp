#include "InputHandler.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
extern bool Active_Third_Camera; // 声明外部变量
InputHandler::InputHandler(Camera* camera, CameraController* camera_controller, Scene* scene, Stob* controlledStob, Player* player, UI* gameUI, int windowWidth, int windowHeight)
    : camera(camera), camera_controller(camera_controller), scene(scene), controlledStob(controlledStob), player(player), gameUI(gameUI), windowWidth(windowWidth), windowHeight(windowHeight),
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
    else if (key == 32) { // Spacebar - jump
        if (Active_Third_Camera) {
            player->jump();
        } else {
            camera->jump();
        }
    }
}

void InputHandler::handleKeyRelease(unsigned char key) {
    keys[key] = false;
}

void InputHandler::handleMouseClick(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            Vector3 position, direction;

            if (Active_Third_Camera) {
                // Third-person camera: shoot from Stob position following the vision direction (crosshair)
                position = player->getPosition() + Vector3(0.0f, 0.5f, 0.0f);
                direction = -player->getVisionDirection();
            } else {
                // First-person camera: shoot from camera position in look direction
                position = Vector3(camera->getX(), camera->getY(), camera->getZ());
                direction = camera->getLookDirection();
            }

            scene->fireBullet(position, direction.normalized());
        }
    }
}

void InputHandler::handleMouseMotion(int x, int y) {
    
    if (!mouseCaptured) {
        return; // Don't process mouse movement when not captured
    }

    if (Active_Third_Camera == false){ 
        
        float centerX = windowWidth / 2.0f;
        float centerY = windowHeight / 2.0f;
        gameUI->setCrossPosition(centerX, centerY);//第一人称准星在屏幕中心
        if (firstMouse) {
            lastMouseX = x;
            lastMouseY = y;
            firstMouse = false;
        }
        
    
        float deltaX = x - lastMouseX;
        float deltaY = y - lastMouseY;
    
        lastMouseX = x;
        lastMouseY = y;

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
        gameUI->setCrossPosition(x, y);//第三人称准星在鼠标
        float centerX = windowWidth / 2.0f;
        float centerY = windowHeight / 2.0f;
        
        float deltaX = x - centerX;
        float deltaY = y - centerY;

        //第三人称，角色朝向就是屏幕中心向鼠标坐标的方向
         Vector3 dir;
        float yaw = atan2f(deltaY, deltaX); // 计算朝向角度
        dir.x = cosf(yaw);
        dir.z = sinf(yaw);
        player->updateVisionDirection(dir);
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

            player->setPosition(Vector3(camera->getX(), camera->getY()-1.5f, camera->getZ()));
            if (player->checkSceneCollision()) {
                camera->move(-forward, -right);
            }
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

            // Move the Stob (player character)
            // controlledStob->move_absolute(moveX, 0.0f, moveY);
            player->moveAbsolute(moveX, 0.0f, moveY);
            if(player->checkSceneCollision()) {
                player->moveAbsolute(-moveX, 0.0f, -moveY);
                camera_controller->movePlayerAbsolute(-moveX, -moveY);
            }

            // Sync camera position with Player's position
            Vector3 playerPos = player->getPosition();
            camera_controller->setPlayerPosition(playerPos.x, playerPos.z);

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
    if (Active_Third_Camera) {
        // Switching from third-person to first-person
        // Move first-person camera to Stob's position (eye level)
        Vector3 playerPos = player->getPosition();
        camera->setPosition(playerPos.x, playerPos.y + 1.5f, playerPos.z);

        // Hide Stob in first-person view
        player->setVisible(false);
    } else {
        // Switching from first-person to third-person
        // Move Stob to first-person camera's position
        float camX = camera->getX();
        float camY = camera->getY();
        float camZ = camera->getZ();

        // Set Stob position (ground level)
        // Vector3 newStobPos(camX, 0.0f, camZ);
        // controlledStob->setPosition(newStobPos);
        Vector3 newPlayerPos(camX, 0.0f, camZ);
        player->setPosition(newPlayerPos);

        // Sync third-person camera controller
        camera_controller->setPlayerPosition(camX, camZ);

        // Show Stob in third-person view
        controlledStob->setVisible(true);
        player->setVisible(true);
    }

    Active_Third_Camera = !Active_Third_Camera;
}
