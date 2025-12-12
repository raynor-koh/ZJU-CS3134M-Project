#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "UI.h"
#include "Stob.h"
#include "Camera.h"
#include "Scene.h"
#include "InputHandler.h"
#include "camera_controller.h"
#include "ScreenRecorder.h"
#include "Target.h"
#include "Bullet.h"

// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

//全局变量控制当前摄像机
bool Active_Third_Camera = false;            // 当前活跃的摄像机false:第一人称,true:第三人称

// Global objects
UI* gameUI = nullptr;
Camera* camera = nullptr;
CameraController* camera_controller = nullptr;
Scene* scene = nullptr;
Stob* stob_0 = nullptr;
Player* player = nullptr;
InputHandler* inputHandler = nullptr;
ScreenRecorder* screenRecorder = nullptr;

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Set up lighting
    GLfloat lightPos[] = {10.0f, 20.0f, 10.0f, 1.0f};
    GLfloat lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    // Background color (sky blue)
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 100.0);

    // Set up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply camera view
    if(Active_Third_Camera == false) {
        camera->applyView();
    }
    else {
        camera_controller->applyView();
    }
    // Draw the scene
    scene->draw();
    //Draw the controlled Stob
    //stob_0->draw();
    player->draw();
    gameUI->drawCross();
    glutSwapBuffers();

    // Capture frame for screen recording if recording is active
    if (screenRecorder && screenRecorder->isRecording()) {
        screenRecorder->captureFrame();
    }
}

void update(int value) {
    float deltaTime = 0.016f; // ~60 FPS = 16ms

    inputHandler->update();
    scene->update(deltaTime);

    // Update physics (jumping, gravity)
    camera->update(deltaTime);
    player->update(deltaTime);

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    // Handle screen recording toggle (R key)
    if (key == 'r' || key == 'R') {
        if (screenRecorder) {
            if (screenRecorder->isRecording()) {
                screenRecorder->stopRecording();
            } else {
                screenRecorder->startRecording(""); // Empty string for auto-generated filename
            }
        }
        return;
    }

    inputHandler->handleKeyPress(key);
}

void keyboardUp(unsigned char key, int x, int y) {
    inputHandler->handleKeyRelease(key);
}

void mouseMotion(int x, int y) {
    inputHandler->handleMouseMotion(x, y);
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    inputHandler->setWindowSize(width, height);
}

void mouse(int button, int state, int x, int y) {
    inputHandler->handleMouseClick(button, state, x, y);
}

void cleanup() {
    delete screenRecorder;
    delete camera_controller;
    delete camera;
    delete scene;
    delete stob_0;
    delete inputHandler;
    delete gameUI;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("FPS Game");

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    // Initialize OpenGL settings
    initOpenGL();

    // Create game objects
    scene = new Scene();
    scene->initialize();

    // Add a stationary target for shooting practice
    Target* target1 = new Target(Vector3(0.0f, 1.5f, -10.0f), Vector3(2.0f, 2.0f, 0.5f), Color(1.0f, 0.0f, 0.0f));
    scene->addTarget(target1);

    stob_0 = new Stob(Vector3(5.0f, 0.0f, 5.0f), 2.0f, 1.0f, Color(1.0f, 0.0f, 0.0f));
    stob_0->setTestDraw(true);//开启方向箭头绘制
    stob_0->setScene(scene);  // Set scene for collision detection
    stob_0->setVisible(false); // Hide Stob initially (first-person view)

    camera = new Camera(5.0f, 1.5f, 5.0f);  // Position camera at eye level above Stob
    camera->setScene(scene);  // Set scene for collision detection
    camera->setCollisionRadius(0.5f);  // Set player collision radius

    camera_controller = new CameraController(WINDOW_WIDTH, WINDOW_HEIGHT);
    camera_controller->setPlayerPosition(5.0f, 5.0f);  // Sync with Stob's starting position
    player = new Player(scene);
    gameUI = new UI(WINDOW_WIDTH, WINDOW_HEIGHT);
    inputHandler = new InputHandler(camera, camera_controller, scene, stob_0, player, gameUI, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialize screen recorder (30 FPS)
    screenRecorder = new ScreenRecorder(WINDOW_WIDTH, WINDOW_HEIGHT, 30);

    // Hide and center cursor
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutMouseFunc(mouse);
    glutTimerFunc(0, update, 0);

    // Print controls
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move around" << std::endl;
    std::cout << "  Spacebar - Jump" << std::endl;
    std::cout << "  Mouse - Look around / Control player direction" << std::endl;
    std::cout << "  Left Click - Fire bullet (works in both camera modes)" << std::endl;
    std::cout << "  C - Switch between first-person and third-person camera" << std::endl;
    std::cout << "  Tab - Toggle mouse capture (free cursor for other windows)" << std::endl;
    std::cout << "  R - Start/Stop screen recording (saves to project root videos/ folder)" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;

    // Cleanup on exit
    atexit(cleanup);

    glutMainLoop();
    return 0;
}
