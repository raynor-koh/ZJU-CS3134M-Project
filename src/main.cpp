#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>

#include "Camera.h"
#include "Scene.h"
#include "InputHandler.h"
#include "camera_controller.h"
// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

//全局变量控制当前摄像机
bool Active_Third_Camera = false;            // 当前活跃的摄像机false:第一人称,true:第三人称

// Global objects
Camera* camera = nullptr;
CameraController* camera_controller = nullptr;
Scene* scene = nullptr;
InputHandler* inputHandler = nullptr;

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

    glutSwapBuffers();
}

void update(int value) {
    inputHandler->update();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
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

void cleanup() {
    delete camera_controller;
    delete camera;
    delete scene;
    delete inputHandler;
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
    camera = new Camera(0.0f, 2.0f, 10.0f);
    camera_controller = new CameraController(WINDOW_WIDTH, WINDOW_HEIGHT);
    scene = new Scene();
    scene->initialize();
    inputHandler = new InputHandler(camera, camera_controller, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Hide and center cursor
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutTimerFunc(0, update, 0);

    // Print controls
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move around" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Tab - Toggle mouse capture (free cursor for other windows)" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;

    // Cleanup on exit
    atexit(cleanup);

    glutMainLoop();
    return 0;
}
