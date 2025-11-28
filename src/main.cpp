#define _USE_MATH_DEFINES
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <iostream>

// Window dimensions
int windowWidth = 1280;
int windowHeight = 720;

// Camera properties
float cameraX = 0.0f, cameraY = 2.0f, cameraZ = 10.0f;
float cameraYaw = 0.0f;   // Horizontal rotation
float cameraPitch = 0.0f; // Vertical rotation

// Movement
float moveSpeed = 0.2f;
bool keys[256] = {false};

// Mouse control
int lastMouseX = windowWidth / 2;
int lastMouseY = windowHeight / 2;
bool firstMouse = true;
float mouseSensitivity = 0.1f;

// Forward declaration
void drawBox(float x, float y, float z, float width, float height, float depth,
             float r, float g, float b);
void drawGround();

void init() {
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

    // Hide cursor and center it
    glutSetCursor(GLUT_CURSOR_NONE);
    glutWarpPointer(windowWidth / 2, windowHeight / 2);
}

void drawBox(float x, float y, float z, float width, float height, float depth,
             float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(r, g, b);
    glScalef(width, height, depth);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawGround() {
    glPushMatrix();
    glColor3f(0.2f, 0.6f, 0.2f); // Green grass color

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal pointing up

    float groundSize = 50.0f;
    glVertex3f(-groundSize, 0.0f, -groundSize);
    glVertex3f(groundSize, 0.0f, -groundSize);
    glVertex3f(groundSize, 0.0f, groundSize);
    glVertex3f(-groundSize, 0.0f, groundSize);

    glEnd();
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)windowWidth / windowHeight, 0.1, 100.0);

    // Set up modelview matrix (camera)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Calculate camera direction
    float lookX = cameraX + cos(cameraPitch * M_PI / 180.0f) * sin(cameraYaw * M_PI / 180.0f);
    float lookY = cameraY + sin(cameraPitch * M_PI / 180.0f);
    float lookZ = cameraZ - cos(cameraPitch * M_PI / 180.0f) * cos(cameraYaw * M_PI / 180.0f);

    gluLookAt(cameraX, cameraY, cameraZ,  // Camera position
              lookX, lookY, lookZ,          // Look at point
              0.0f, 1.0f, 0.0f);           // Up vector

    // Draw the ground
    drawGround();

    // Draw some boxes in the scene
    drawBox(0.0f, 1.0f, 0.0f, 2.0f, 2.0f, 2.0f, 0.8f, 0.3f, 0.3f);   // Red box at center
    drawBox(5.0f, 0.5f, -3.0f, 1.0f, 1.0f, 1.0f, 0.3f, 0.3f, 0.8f);  // Blue box
    drawBox(-4.0f, 1.5f, 2.0f, 3.0f, 3.0f, 3.0f, 0.8f, 0.8f, 0.3f);  // Yellow box
    drawBox(8.0f, 1.0f, 5.0f, 2.0f, 2.0f, 2.0f, 0.5f, 0.2f, 0.6f);   // Purple box
    drawBox(-7.0f, 0.75f, -6.0f, 1.5f, 1.5f, 1.5f, 0.3f, 0.7f, 0.5f); // Teal box
    drawBox(3.0f, 2.0f, 8.0f, 4.0f, 4.0f, 4.0f, 0.9f, 0.5f, 0.2f);   // Orange box

    glutSwapBuffers();
}

void update(int value) {
    // Handle WASD movement
    float moveX = 0.0f, moveZ = 0.0f;

    if (keys['w'] || keys['W']) {
        moveX += sin(cameraYaw * M_PI / 180.0f);
        moveZ -= cos(cameraYaw * M_PI / 180.0f);
    }
    if (keys['s'] || keys['S']) {
        moveX -= sin(cameraYaw * M_PI / 180.0f);
        moveZ += cos(cameraYaw * M_PI / 180.0f);
    }
    if (keys['a'] || keys['A']) {
        moveX -= cos(cameraYaw * M_PI / 180.0f);
        moveZ -= sin(cameraYaw * M_PI / 180.0f);
    }
    if (keys['d'] || keys['D']) {
        moveX += cos(cameraYaw * M_PI / 180.0f);
        moveZ += sin(cameraYaw * M_PI / 180.0f);
    }

    cameraX += moveX * moveSpeed;
    cameraZ += moveZ * moveSpeed;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // ~60 FPS
}

void keyboard(unsigned char key, int x, int y) {
    keys[key] = true;

    if (key == 27) { // ESC key
        exit(0);
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void mouseMotion(int x, int y) {
    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
    }

    float deltaX = x - lastMouseX;
    float deltaY = y - lastMouseY;

    lastMouseX = x;
    lastMouseY = y;

    cameraYaw += deltaX * mouseSensitivity;
    cameraPitch -= deltaY * mouseSensitivity;

    // Clamp pitch to prevent flipping
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    // Re-center mouse cursor
    if (abs(x - windowWidth / 2) > 100 || abs(y - windowHeight / 2) > 100) {
        glutWarpPointer(windowWidth / 2, windowHeight / 2);
        lastMouseX = windowWidth / 2;
        lastMouseY = windowHeight / 2;
    }
}

void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("FPS Game");

    // Initialize GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW Error: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutTimerFunc(0, update, 0);

    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move around" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;

    glutMainLoop();
    return 0;
}
