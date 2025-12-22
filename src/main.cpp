#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <filesystem>
#include "UI.h"
#include "Stob.h"
#include "Camera.h"
#include "Scene.h"
#include "InputHandler.h"
#include "camera_controller.h"
#include "ScreenRecorder.h"
#include "Target.h"
#include "Bullet.h"
#include "Lighting.h"
#include "MeshIO.h"
#include "Shapes.h"

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
Lighting* lighting = nullptr;

void initOpenGL() {
    glEnable(GL_DEPTH_TEST);

    // NOTE: Lighting is now controlled by the Lighting class
    // Don't enable GL_LIGHTING here - let Lighting::apply() handle it

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

    // CRITICAL: Apply lighting AFTER camera view is set
    // This allows light position to be transformed by the current modelview (headlight mode)
    if (lighting) {
        // Update headlight position if in headlight mode
        if (lighting->isHeadlightMode()) {
            if (Active_Third_Camera == false) {
                // First-person: light follows camera
                Vector3 lookDir = camera->getLookDirection();
                lighting->updateHeadlight(camera->getX(), camera->getY(), camera->getZ(),
                                         lookDir.x, lookDir.y, lookDir.z);
            } else {
                // Third-person: light follows player
                Vector3 playerPos = player->getPosition();
                Vector3 visionDir = -player->getVisionDirection();
                lighting->updateHeadlight(playerPos.x, playerPos.y + 1.0f, playerPos.z,
                                         visionDir.x, visionDir.y, visionDir.z);
            }
        }
        lighting->apply();
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

    // Handle screenshot (P key)
    if (key == 'p' || key == 'P') {
        if (screenRecorder) {
            screenRecorder->takeScreenshot();
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

void specialKey(int key, int x, int y) {
    inputHandler->handleSpecialKey(key, x, y);
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    inputHandler->setWindowSize(width, height);
}

void mouse(int button, int state, int x, int y) {
    inputHandler->handleMouseClick(button, state, x, y);
}

void cleanup() {
    delete lighting;
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

    // Create lighting system (enabled by default with headlight mode)
    lighting = new Lighting();
    // Headlight mode is enabled by default - light follows active camera
    // Press 'L' to toggle lighting on/off
    scene->setLighting(lighting);
    std::cout << "Lighting initialized: Headlight mode ENABLED (follows camera)" << std::endl;

    // Add a stationary target for shooting practice
    Target* target1 = new Target(Vector3(0.0f, 1.5f, -10.0f), Vector3(2.0f, 2.0f, 0.5f), Color(1.0f, 0.0f, 0.0f));
    scene->addTarget(target1);

    // === Mesh Import/Export Demo ===
    std::cout << "\n=== Mesh Import/Export Demo ===" << std::endl;

    // Create mesh directory structure
    std::filesystem::create_directories("../../resources/meshes/exported");
    std::filesystem::create_directories("../../resources/meshes/imported");

    // Demo: Export current scene (before adding test shapes)
    bool sceneExportSuccess = MeshIO::exportSceneOBJ(scene, "../../resources/meshes/exported/initial_scene.obj");
    if (sceneExportSuccess) {
        std::cout << "Scene exported to resources/meshes/exported/initial_scene.obj" << std::endl;
    }

    // Demo: Create and export a test cube
    auto testCube = std::make_shared<Cube>(
        Vector3(15.0f, 2.0f, 15.0f),
        Vector3(3.0f, 3.0f, 3.0f),
        Color(1.0f, 0.5f, 0.0f)
    );
    scene->addShape(testCube);
    MeshIO::exportShapeOBJ(testCube.get(), "../../resources/meshes/exported/test_cube.obj");
    std::cout << "Test cube exported to resources/meshes/exported/test_cube.obj" << std::endl;

    // Demo: Create and export a test sphere
    auto testSphere = std::make_shared<Sphere>(
        Vector3(-15.0f, 3.0f, 15.0f),
        4.0f,  // diameter
        Color(0.0f, 0.5f, 1.0f)
    );
    scene->addShape(testSphere);
    MeshIO::exportShapeOBJ(testSphere.get(), "../../resources/meshes/exported/test_sphere.obj");
    std::cout << "Test sphere exported to resources/meshes/exported/test_sphere.obj" << std::endl;

    // Demo: Create and export a test cylinder
    auto testCylinder = std::make_shared<Cylinder>(
        Vector3(-15.0f, 2.0f, -15.0f),
        4.0f,  // height
        2.0f,  // diameter
        Color(0.5f, 1.0f, 0.0f)
    );
    scene->addShape(testCylinder);
    MeshIO::exportShapeOBJ(testCylinder.get(), "../../resources/meshes/exported/test_cylinder.obj");
    std::cout << "Test cylinder exported to resources/meshes/exported/test_cylinder.obj" << std::endl;

    // Demo: Import OBJ (if file exists)
    std::cout << "\n=== Mesh Import Demo ===" << std::endl;
    if (std::filesystem::exists("../../resources/meshes/imported/test.obj")) {
        auto imported = MeshIO::importOBJ("../../resources/meshes/imported/test.obj");
        if (imported) {
            imported->setPosition(Vector3(-10.0f, 2.0f, -10.0f));
            imported->setColor(Color(0.9f, 0.2f, 0.7f));
            scene->addShape(imported);
            std::cout << "Mesh imported from resources/meshes/imported/test.obj and added to scene!" << std::endl;
        }
    } else {
        std::cout << "No test.obj file found in resources/meshes/imported/ - skipping import demo" << std::endl;
        std::cout << "You can place an OBJ file there to test importing." << std::endl;
    }

    // Export final scene with all test shapes
    MeshIO::exportSceneOBJ(scene, "../../resources/meshes/exported/final_scene.obj");
    std::cout << "Final scene with test shapes exported to resources/meshes/exported/final_scene.obj" << std::endl;
    std::cout << "==============================\n" << std::endl;

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
    inputHandler->setLighting(lighting);

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
    glutSpecialFunc(specialKey);  // For arrow keys and PageUp/PageDown
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
    std::cout << "  P - Take screenshot (saves PNG to project root pics/ folder)" << std::endl;
    std::cout << std::endl;
    std::cout << "Lighting Controls (Advanced - lighting is ON by default):" << std::endl;
    std::cout << "  L - Toggle lighting on/off" << std::endl;
    std::cout << "  [ / ] - Decrease/Increase light intensity" << std::endl;
    std::cout << "  (Headlight mode: light automatically follows camera)" << std::endl;
    std::cout << std::endl;
    std::cout << "  ESC - Exit" << std::endl;

    // Cleanup on exit
    atexit(cleanup);

    glutMainLoop();
    return 0;
}
