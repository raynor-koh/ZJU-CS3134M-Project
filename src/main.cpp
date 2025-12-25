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
#include "FreeCamera.h"
#include "ScreenRecorder.h"
#include "Target.h"
#include "Bullet.h"
#include "Lighting.h"
#include "MeshIO.h"
#include "Shapes.h"
#include "GameState.h"
#include "EnemyManager.h"

// Window dimensions
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

//全局变量控制当前摄像机
bool Active_Third_Camera = false;            // 当前活跃的摄像机false:第一人称,true:第三人称

// Game state management
GameState gameState = GameState::PLAYING;

// Global objects
UI* gameUI = nullptr;
Camera* camera = nullptr;
CameraController* camera_controller = nullptr;
FreeCamera* free_camera = nullptr;
Scene* scene = nullptr;
Stob* stob_0 = nullptr;
Player* player = nullptr;
InputHandler* inputHandler = nullptr;
ScreenRecorder* screenRecorder = nullptr;
Lighting* lighting = nullptr;
EnemyManager* enemyManager = nullptr;

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
    if(inputHandler && inputHandler->isFreeCameraActive()) {
        // Free Camera / Spectator mode
        free_camera->applyView();
    }
    else if(Active_Third_Camera == false) {
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
    gameUI->drawEditorUI();

    // Draw Game Over overlay if dead
    if (gameState == GameState::GAME_OVER) {
        // Save OpenGL state
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        // Set up orthographic projection for 2D overlay
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glOrtho(0, viewport[2], 0, viewport[3], -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        float screenW = static_cast<float>(viewport[2]);
        float screenH = static_cast<float>(viewport[3]);
        float centerX = screenW / 2.0f;
        float centerY = screenH / 2.0f;

        // Draw full-screen dark overlay with gradient effect
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Darker at edges, lighter at center (vignette effect)
        glBegin(GL_QUADS);
        // Dark overlay
        glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
        glVertex2f(0, 0);
        glVertex2f(screenW, 0);
        glVertex2f(screenW, screenH);
        glVertex2f(0, screenH);
        glEnd();

        // Central panel - larger and more prominent
        float panelWidth = 400.0f;
        float panelHeight = 180.0f;

        // Panel shadow (offset dark rectangle)
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
        glBegin(GL_QUADS);
        glVertex2f(centerX - panelWidth/2 + 8, centerY - panelHeight/2 - 8);
        glVertex2f(centerX + panelWidth/2 + 8, centerY - panelHeight/2 - 8);
        glVertex2f(centerX + panelWidth/2 + 8, centerY + panelHeight/2 - 8);
        glVertex2f(centerX - panelWidth/2 + 8, centerY + panelHeight/2 - 8);
        glEnd();

        // Main panel background (dark gray gradient)
        glBegin(GL_QUADS);
        glColor4f(0.15f, 0.15f, 0.18f, 0.95f);  // Top - slightly lighter
        glVertex2f(centerX - panelWidth/2, centerY + panelHeight/2);
        glVertex2f(centerX + panelWidth/2, centerY + panelHeight/2);
        glColor4f(0.08f, 0.08f, 0.1f, 0.95f);   // Bottom - darker
        glVertex2f(centerX + panelWidth/2, centerY - panelHeight/2);
        glVertex2f(centerX - panelWidth/2, centerY - panelHeight/2);
        glEnd();

        glDisable(GL_BLEND);

        // Panel border - double line effect
        glColor3f(0.4f, 0.1f, 0.1f);  // Dark red outer
        glLineWidth(4.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(centerX - panelWidth/2, centerY - panelHeight/2);
        glVertex2f(centerX + panelWidth/2, centerY - panelHeight/2);
        glVertex2f(centerX + panelWidth/2, centerY + panelHeight/2);
        glVertex2f(centerX - panelWidth/2, centerY + panelHeight/2);
        glEnd();

        glColor3f(0.8f, 0.2f, 0.2f);  // Brighter red inner
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(centerX - panelWidth/2 + 4, centerY - panelHeight/2 + 4);
        glVertex2f(centerX + panelWidth/2 - 4, centerY - panelHeight/2 + 4);
        glVertex2f(centerX + panelWidth/2 - 4, centerY + panelHeight/2 - 4);
        glVertex2f(centerX - panelWidth/2 + 4, centerY + panelHeight/2 - 4);
        glEnd();
        glLineWidth(1.0f);

        // Decorative line under title
        glColor3f(0.6f, 0.15f, 0.15f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(centerX - 120.0f, centerY + 15.0f);
        glVertex2f(centerX + 120.0f, centerY + 15.0f);
        glEnd();
        glLineWidth(1.0f);

        // "GAME OVER" title - large text
        glColor3f(0.9f, 0.2f, 0.2f);  // Bright red
        const char* titleText = "GAME OVER";
        // GLUT_BITMAP_TIMES_ROMAN_24 is larger
        int titleCharWidth = 14;  // Approximate width per character
        float titleX = centerX - (strlen(titleText) * titleCharWidth) / 2.0f;
        glRasterPos2f(titleX, centerY + 40.0f);
        for (const char* c = titleText; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
        }

        // "You were defeated" message
        glColor3f(0.7f, 0.7f, 0.7f);
        const char* defeatText = "You were defeated by the enemies";
        int defeatCharWidth = 9;
        float defeatX = centerX - (strlen(defeatText) * defeatCharWidth) / 2.0f;
        glRasterPos2f(defeatX, centerY - 15.0f);
        for (const char* c = defeatText; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }

        // "Press ESC to exit" instruction
        glColor3f(0.5f, 0.5f, 0.5f);
        const char* exitText = "Press ESC to exit";
        int exitCharWidth = 8;
        float exitX = centerX - (strlen(exitText) * exitCharWidth) / 2.0f;
        glRasterPos2f(exitX, centerY - 55.0f);
        for (const char* c = exitText; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }

        // Restore OpenGL state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    glutSwapBuffers();

    // Capture frame for screen recording if recording is active
    if (screenRecorder && screenRecorder->isRecording()) {
        screenRecorder->captureFrame();
    }
}

void update(int value) {
    float deltaTime = 0.016f; // ~60 FPS = 16ms

    // Update game state based on free camera mode
    if (inputHandler && inputHandler->isFreeCameraActive() || inputHandler->isEditModeActive()) {
        if (gameState == GameState::PLAYING) {
            gameState = GameState::PAUSED;
        }
    } else {
        if (gameState == GameState::PAUSED) {
            gameState = GameState::PLAYING;
        }
    }

    // Always update input handler (for camera controls)
    inputHandler->update();

    // Only update gameplay when PLAYING
    if (gameState == GameState::PLAYING) {
        scene->update(deltaTime);

        // Update physics (jumping, gravity)
        camera->update(deltaTime);
        player->update(deltaTime);

        // Update enemy manager (spawning, movement)
        if (enemyManager && player) {
            Vector3 playerPos = player->getPosition();
            enemyManager->update(deltaTime, gameState, playerPos);

            // Check for player damage from enemies
            float damage = enemyManager->checkPlayerCollision(player, deltaTime);
            if (damage > 0.0f) {
                player->takeDamage(damage);

                // Check for game over
                if (!player->isAlive()) {
                    gameState = GameState::GAME_OVER;
                    std::cout << "\n=== GAME OVER ===\n" << std::endl;
                    std::cout << "You were killed by enemies!" << std::endl;
                    std::cout << "Press ESC to exit." << std::endl;
                }
            }
        }
    }

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
    // Mouse wheel events (buttons 3 and 4) - legacy GLUT
    if (button == 3 || button == 4) {
        inputHandler->handleMouseWheel(button, state, x, y);
    } else {
        inputHandler->handleMouseClick(button, state, x, y);
    }
}

void mouseWheel(int wheel, int direction, int x, int y) {
    // Freeglut mouse wheel callback
    // direction > 0 = scroll up (zoom in), direction < 0 = scroll down (zoom out)
    int button = (direction > 0) ? 3 : 4;  // Convert to legacy button numbers
    inputHandler->handleMouseWheel(button, GLUT_DOWN, x, y);
}

void cleanup() {
    delete enemyManager;
    delete lighting;
    delete screenRecorder;
    delete camera_controller;
    delete free_camera;
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
        Vector3(15.0f, 1.5f, 15.0f),  // Y = height/2 = 3.0/2 = 1.5 (sits on ground)
        Vector3(3.0f, 3.0f, 3.0f),
        Color(1.0f, 0.5f, 0.0f)
    );
    scene->addShape(testCube);
    MeshIO::exportShapeOBJ(testCube.get(), "../../resources/meshes/exported/test_cube.obj");
    std::cout << "Test cube exported to resources/meshes/exported/test_cube.obj" << std::endl;

    // Demo: Create and export a test sphere
    auto testSphere = std::make_shared<Sphere>(
        Vector3(-15.0f, 2.0f, 15.0f),  // Y = diameter/2 = 4.0/2 = 2.0 (sits on ground)
        4.0f,  // diameter
        Color(0.0f, 0.5f, 1.0f)
    );
    scene->addShape(testSphere);
    MeshIO::exportShapeOBJ(testSphere.get(), "../../resources/meshes/exported/test_sphere.obj");
    std::cout << "Test sphere exported to resources/meshes/exported/test_sphere.obj" << std::endl;

    // Demo: Create and export a test cylinder
    auto testCylinder = std::make_shared<Cylinder>(
        Vector3(-15.0f, 2.0f, -15.0f),  // Y = height/2 = 4.0/2 = 2.0 (sits on ground)
        4.0f,  // height
        2.0f,  // diameter
        Color(0.5f, 1.0f, 0.0f)
    );
    scene->addShape(testCylinder);
    MeshIO::exportShapeOBJ(testCylinder.get(), "../../resources/meshes/exported/test_cylinder.obj");
    std::cout << "Test cylinder exported to resources/meshes/exported/test_cylinder.obj" << std::endl;

    // Demo: Create and export a test cone
    auto testCone = std::make_shared<Cone>(
        Vector3(20.0f, 1.5f, 0.0f),  // Y = height/2 = 3.0/2 = 1.5 (sits on ground)
        3.0f,  // height
        2.0f,  // base diameter
        Color(1.0f, 0.8f, 0.0f)
    );
    scene->addShape(testCone);
    MeshIO::exportShapeOBJ(testCone.get(), "../../resources/meshes/exported/test_cone.obj");
    std::cout << "Test cone exported to resources/meshes/exported/test_cone.obj" << std::endl;

    // Demo: Create and export a test prism (hexagonal)
    auto testPrism = std::make_shared<Prism>(
        Vector3(20.0f, 1.5f, -10.0f),  // Y = height/2 = 3.0/2 = 1.5 (sits on ground)
        3.0f,  // height
        2.5f,  // diameter
        6,     // sides (hexagon)
        Color(0.0f, 1.0f, 0.8f)
    );
    scene->addShape(testPrism);
    MeshIO::exportShapeOBJ(testPrism.get(), "../../resources/meshes/exported/test_prism.obj");
    std::cout << "Test prism (hexagon) exported to resources/meshes/exported/test_prism.obj" << std::endl;

    // Demo: Create and export a test frustum (square pyramid frustum)
    auto testFrustum = std::make_shared<Frustum>(
        Vector3(20.0f, 1.5f, 10.0f),  // Y = height/2 = 3.0/2 = 1.5 (sits on ground)
        3.0f,  // height
        3.0f,  // bottom diameter
        1.5f,  // top diameter
        4,     // sides (square)
        Color(1.0f, 0.4f, 0.8f)
    );
    scene->addShape(testFrustum);
    MeshIO::exportShapeOBJ(testFrustum.get(), "../../resources/meshes/exported/test_frustum.obj");
    std::cout << "Test frustum (square) exported to resources/meshes/exported/test_frustum.obj" << std::endl;

    // Demo: Import OBJ (if file exists)
    std::cout << "\n=== Mesh Import Demo ===" << std::endl;
    if (std::filesystem::exists("../../resources/meshes/imported/test.obj")) {
        auto imported = MeshIO::importOBJ("../../resources/meshes/imported/test.obj");
        if (imported) {
            imported->setPosition(Vector3(-10.0f, 0.0f, -10.0f));  // Place on ground
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

    // Create Free Camera / Spectator mode camera
    free_camera = new FreeCamera(5.0f, 10.0f, 5.0f);  // Start elevated above scene

    player = new Player(scene);
    // IMPORTANT: Sync player position with camera at startup to prevent position mismatch
    player->setPosition(Vector3(camera->getX(), 0.0f, camera->getZ()));
    player->setVisible(false);  // Hidden in first-person mode

    gameUI = new UI(WINDOW_WIDTH, WINDOW_HEIGHT);
    inputHandler = new InputHandler(camera, camera_controller, free_camera, scene, stob_0, player, gameUI, WINDOW_WIDTH, WINDOW_HEIGHT);
    inputHandler->setLighting(lighting);

    // Initialize enemy manager
    enemyManager = new EnemyManager(scene);
    enemyManager->setSpawnInterval(5.0f);    // Spawn every 5 seconds
    enemyManager->setMinSpawnRadius(10.0f);  // At least 10 units from player
    enemyManager->setMaxSpawnRadius(30.0f);  // At most 30 units from player
    enemyManager->setMaxEnemies(6);          // Maximum 6 enemies at once
    enemyManager->setEnemySpeed(1.5f);       // Enemies move at 1.5 units/second
    std::cout << "Enemy system initialized: spawning every 5s, max 6 enemies" << std::endl;

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
    glutPassiveMotionFunc(mouseMotion);  // Mouse movement without buttons
    glutMotionFunc(mouseMotion);         // Mouse movement WITH buttons (for dragging)
    glutMouseFunc(mouse);

    // Try to register mouse wheel callback (only available in freeglut 2.4+)
    #ifdef GLUT_WHEEL_UP
    glutMouseWheelFunc(mouseWheel);
    std::cout << "Mouse wheel callback registered (freeglut)" << std::endl;
    #else
    std::cout << "Mouse wheel via buttons 3/4 (legacy GLUT)" << std::endl;
    #endif

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
    std::cout << "Free Camera / Spectator Mode (Photo Mode):" << std::endl;
    std::cout << "  V - Toggle Free Camera ON/OFF (freezes player, shows cursor)" << std::endl;
    std::cout << "  Alt + Left Mouse Drag - Orbit camera around scene" << std::endl;
    std::cout << "  Alt + Right Mouse Drag - Pan camera (move sideways/up/down)" << std::endl;
    std::cout << "  Mouse Wheel OR +/- Keys - Zoom in/out" << std::endl;
    std::cout << "  F - Zoom to Fit (frame entire scene in view)" << std::endl;
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
