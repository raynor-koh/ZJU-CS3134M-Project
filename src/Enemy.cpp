#include "Enemy.h"
#include <iostream>
#include <cmath>

namespace {
    constexpr float ENEMY_SCALE = 0.6f;
    constexpr float BODY_DIAMETER = 3.6f * ENEMY_SCALE;
    constexpr float BODY_Y_OFFSET = 1.0f * ENEMY_SCALE;
    constexpr float HEAD_DIAMETER = 2.4f * ENEMY_SCALE;
    constexpr float HEAD_Y_OFFSET = 3.0f * ENEMY_SCALE;
    constexpr float ARM_OFFSET_X = 1.8f * ENEMY_SCALE;
    constexpr float ARM_OFFSET_Y = 1.5f * ENEMY_SCALE;
    constexpr float ARM_LENGTH = 1.5f * ENEMY_SCALE;
    constexpr float ARM_DIAMETER = 0.16f * ENEMY_SCALE;
    constexpr float NOSE_OFFSET_Z = 1.2f * ENEMY_SCALE;
    constexpr float NOSE_LENGTH = 0.5f * ENEMY_SCALE;
    constexpr float NOSE_DIAMETER = 0.16f * ENEMY_SCALE;
    constexpr float BUTTON_OFFSET_Z = 1.8f * ENEMY_SCALE;
    constexpr float BUTTON1_Y_OFFSET = 1.8f * ENEMY_SCALE;
    constexpr float BUTTON2_Y_OFFSET = 1.3f * ENEMY_SCALE;
    constexpr float BUTTON3_Y_OFFSET = 0.8f * ENEMY_SCALE;
    constexpr float BUTTON_DIAMETER = 0.24f * ENEMY_SCALE;
    constexpr float HAT_BRIM_Y_OFFSET = 4.2f * ENEMY_SCALE;
    constexpr float HAT_BRIM_HEIGHT = 0.1f * ENEMY_SCALE;
    constexpr float HAT_BRIM_DIAMETER = 2.0f * ENEMY_SCALE;
    constexpr float HAT_TOP_Y_OFFSET = 4.6f * ENEMY_SCALE;
    constexpr float HAT_TOP_HEIGHT = 0.8f * ENEMY_SCALE;
    constexpr float HAT_TOP_DIAMETER = 1.2f * ENEMY_SCALE;
    constexpr float EYE_OFFSET_X = 0.3f * ENEMY_SCALE;
    constexpr float EYE_OFFSET_Z = 1.1f * ENEMY_SCALE;
    constexpr float EYE_Y_OFFSET = 3.3f * ENEMY_SCALE;
    constexpr float EYE_DIAMETER = 0.2f * ENEMY_SCALE;
    constexpr float HEALTH_BAR_Y_OFFSET = 5.2f * ENEMY_SCALE;
}

Enemy::Enemy(Vector3 ipos, Color icol) : position(ipos), yaw(0.0f), currentHealth(5.0f), maxHealth(5.0f), alive(true) {
    // Main body spheres (white)
    Color white(1.0f, 1.0f, 1.0f);
    bodyLower = new Sphere(ipos + Vector3(0.0f, BODY_Y_OFFSET, 0.0f), BODY_DIAMETER, white);
    head = new Sphere(ipos + Vector3(0.0f, HEAD_Y_OFFSET, 0.0f), HEAD_DIAMETER, white);

    // Stick arms (brown) - use constructor with axis to angle outward
    Color brown(0.4f, 0.25f, 0.1f);
    leftArm = new Cylinder(ipos + Vector3(-ARM_OFFSET_X, ARM_OFFSET_Y, 0.0f), ARM_LENGTH, ARM_DIAMETER,
                           Vector3(-1.0f, 0.2f, 0.0f), 0.0f);  // Point left and slightly up
    leftArm->setColor(brown);
    rightArm = new Cylinder(ipos + Vector3(ARM_OFFSET_X, ARM_OFFSET_Y, 0.0f), ARM_LENGTH, ARM_DIAMETER,
                            Vector3(1.0f, 0.2f, 0.0f), 0.0f);   // Point right and slightly up
    rightArm->setColor(brown);

    // Carrot nose (orange) - using Cylinder since Cone doesn't support rotation
    Color orange(1.0f, 0.5f, 0.0f);
    // Use Cylinder constructor with axis parameter to point forward (Z direction)
    nose = new Cylinder(ipos + Vector3(0.0f, HEAD_Y_OFFSET, NOSE_OFFSET_Z), NOSE_LENGTH, NOSE_DIAMETER,
                        Vector3(0.0f, 0.0f, 1.0f), 0.0f);
    nose->setColor(orange);

    // Coal buttons (black)
    Color black(0.1f, 0.1f, 0.1f);
    button1 = new Sphere(ipos + Vector3(0.0f, BUTTON1_Y_OFFSET, BUTTON_OFFSET_Z), BUTTON_DIAMETER, black);
    button2 = new Sphere(ipos + Vector3(0.0f, BUTTON2_Y_OFFSET, BUTTON_OFFSET_Z), BUTTON_DIAMETER, black);
    button3 = new Sphere(ipos + Vector3(0.0f, BUTTON3_Y_OFFSET, BUTTON_OFFSET_Z), BUTTON_DIAMETER, black);

    // Hat (black)
    hatBrim = new Cylinder(ipos + Vector3(0.0f, HAT_BRIM_Y_OFFSET, 0.0f), HAT_BRIM_HEIGHT, HAT_BRIM_DIAMETER, black);
    hatTop = new Cylinder(ipos + Vector3(0.0f, HAT_TOP_Y_OFFSET, 0.0f), HAT_TOP_HEIGHT, HAT_TOP_DIAMETER, black);

    // Eyes (black)
    leftEye = new Sphere(ipos + Vector3(-EYE_OFFSET_X, EYE_Y_OFFSET, EYE_OFFSET_Z), EYE_DIAMETER, black);
    rightEye = new Sphere(ipos + Vector3(EYE_OFFSET_X, EYE_Y_OFFSET, EYE_OFFSET_Z), EYE_DIAMETER, black);
}

Enemy::Enemy(Vector3 ipos, Color icolHead, Color icolBody) : position(ipos), yaw(0.0f), currentHealth(5.0f), maxHealth(5.0f), alive(true) {
    // Main body spheres (white - snowman should always be white)
    Color white(1.0f, 1.0f, 1.0f);
    bodyLower = new Sphere(ipos + Vector3(0.0f, BODY_Y_OFFSET, 0.0f), BODY_DIAMETER, white);
    head = new Sphere(ipos + Vector3(0.0f, HEAD_Y_OFFSET, 0.0f), HEAD_DIAMETER, white);

    // Stick arms (brown)
    Color brown(0.4f, 0.25f, 0.1f);
    leftArm = new Cylinder(ipos + Vector3(-ARM_OFFSET_X, ARM_OFFSET_Y, 0.0f), ARM_LENGTH, ARM_DIAMETER,
                           Vector3(-1.0f, 0.2f, 0.0f), 0.0f);
    leftArm->setColor(brown);
    rightArm = new Cylinder(ipos + Vector3(ARM_OFFSET_X, ARM_OFFSET_Y, 0.0f), ARM_LENGTH, ARM_DIAMETER,
                            Vector3(1.0f, 0.2f, 0.0f), 0.0f);
    rightArm->setColor(brown);

    // Carrot nose (orange)
    Color orange(1.0f, 0.5f, 0.0f);
    nose = new Cylinder(ipos + Vector3(0.0f, HEAD_Y_OFFSET, NOSE_OFFSET_Z), NOSE_LENGTH, NOSE_DIAMETER,
                        Vector3(0.0f, 0.0f, 1.0f), 0.0f);
    nose->setColor(orange);

    // Coal buttons (black)
    Color black(0.1f, 0.1f, 0.1f);
    button1 = new Sphere(ipos + Vector3(0.0f, BUTTON1_Y_OFFSET, BUTTON_OFFSET_Z), BUTTON_DIAMETER, black);
    button2 = new Sphere(ipos + Vector3(0.0f, BUTTON2_Y_OFFSET, BUTTON_OFFSET_Z), BUTTON_DIAMETER, black);
    button3 = new Sphere(ipos + Vector3(0.0f, BUTTON3_Y_OFFSET, BUTTON_OFFSET_Z), BUTTON_DIAMETER, black);

    // Hat (black)
    hatBrim = new Cylinder(ipos + Vector3(0.0f, HAT_BRIM_Y_OFFSET, 0.0f), HAT_BRIM_HEIGHT, HAT_BRIM_DIAMETER, black);
    hatTop = new Cylinder(ipos + Vector3(0.0f, HAT_TOP_Y_OFFSET, 0.0f), HAT_TOP_HEIGHT, HAT_TOP_DIAMETER, black);

    // Eyes (black)
    leftEye = new Sphere(ipos + Vector3(-EYE_OFFSET_X, EYE_Y_OFFSET, EYE_OFFSET_Z), EYE_DIAMETER, black);
    rightEye = new Sphere(ipos + Vector3(EYE_OFFSET_X, EYE_Y_OFFSET, EYE_OFFSET_Z), EYE_DIAMETER, black);
}

Enemy::~Enemy() {
    // Clean up all shape pointers to prevent memory leaks
    delete bodyLower;
    delete head;
    delete leftArm;
    delete rightArm;
    delete nose;
    delete button1;
    delete button2;
    delete button3;
    delete hatBrim;
    delete hatTop;
    delete leftEye;
    delete rightEye;
}

void Enemy::draw() {
    // Main body structure
    bodyLower->draw();
    head->draw();

    // Decorations
    leftArm->draw();
    rightArm->draw();
    nose->draw();
    button1->draw();
    button2->draw();
    button3->draw();
    hatBrim->draw();
    hatTop->draw();
    leftEye->draw();
    rightEye->draw();

    // Health bar (rendered last for proper overlay)
    drawHealthBar();
}

void Enemy::setColor(Color icolor, PartType part) {
    switch (part) {
        case HEAD:
            head->setColor(icolor);
            break;
        case BODY:
            bodyLower->setColor(icolor);
            break;
    }
}

void Enemy::updatePos() {
    // Calculate rotation matrix components
    float cosYaw = cos(yaw);
    float sinYaw = sin(yaw);

    // Helper lambda to rotate a 2D offset around origin
    auto rotateOffset = [cosYaw, sinYaw](float x, float z) -> Vector3 {
        return Vector3(x * cosYaw - z * sinYaw, 0.0f, x * sinYaw + z * cosYaw);
    };

    // Main body (spheres, no rotation needed for position, but stays centered)
    bodyLower->setPosition(position + Vector3(0.0f, BODY_Y_OFFSET, 0.0f));
    head->setPosition(position + Vector3(0.0f, HEAD_Y_OFFSET, 0.0f));

    // Arms (rotate positions and axes)
    Vector3 leftArmOffset = rotateOffset(-ARM_OFFSET_X, 0.0f);
    leftArm->setPosition(position + leftArmOffset + Vector3(0.0f, ARM_OFFSET_Y, 0.0f));

    Vector3 rightArmOffset = rotateOffset(ARM_OFFSET_X, 0.0f);
    rightArm->setPosition(position + rightArmOffset + Vector3(0.0f, ARM_OFFSET_Y, 0.0f));

    // Nose (rotate position)
    Vector3 noseOffset = rotateOffset(0.0f, NOSE_OFFSET_Z);
    nose->setPosition(position + noseOffset + Vector3(0.0f, HEAD_Y_OFFSET, 0.0f));

    // Buttons (rotate positions)
    Vector3 buttonOffset = rotateOffset(0.0f, BUTTON_OFFSET_Z);
    button1->setPosition(position + buttonOffset + Vector3(0.0f, BUTTON1_Y_OFFSET, 0.0f));
    button2->setPosition(position + buttonOffset + Vector3(0.0f, BUTTON2_Y_OFFSET, 0.0f));
    button3->setPosition(position + buttonOffset + Vector3(0.0f, BUTTON3_Y_OFFSET, 0.0f));

    // Hat (centered, no XZ offset)
    hatBrim->setPosition(position + Vector3(0.0f, HAT_BRIM_Y_OFFSET, 0.0f));
    hatTop->setPosition(position + Vector3(0.0f, HAT_TOP_Y_OFFSET, 0.0f));

    // Eyes (rotate positions)
    Vector3 leftEyeOffset = rotateOffset(-EYE_OFFSET_X, EYE_OFFSET_Z);
    leftEye->setPosition(position + leftEyeOffset + Vector3(0.0f, EYE_Y_OFFSET, 0.0f));

    Vector3 rightEyeOffset = rotateOffset(EYE_OFFSET_X, EYE_OFFSET_Z);
    rightEye->setPosition(position + rightEyeOffset + Vector3(0.0f, EYE_Y_OFFSET, 0.0f));
}

void Enemy::takeDamage(float damage) {
    if (alive) currentHealth -= damage;
    if (currentHealth <= 0.0f) alive = false;
}
void Enemy::drawHealthBar() {
    // Get current matrix
    GLdouble modelview[16], projection[16];
    GLint view[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, view);
    
    // Get forward vector
    Vector3 forward(-static_cast<GLfloat>(modelview[2]), -static_cast<GLfloat>(modelview[6]), -static_cast<GLfloat>(modelview[10]));
    
    // Correctly calculate camera position from view matrix
    // The view matrix is organized as:
    // [Rx  Ry  Rz  Tx]
    // [Ux  Uy  Uz  Ty]
    // [Dx  Dy  Dz  Tz]
    // [0   0   0   1 ]
    // Where R=Right, U=Up, D=Direction, T=Translation
    // Camera position = -R*Tx - U*Ty - D*Tz
    Vector3 cameraPos(
        static_cast<GLfloat>(-(modelview[0]*modelview[12] + modelview[1]*modelview[13] + modelview[2]*modelview[14])),
        static_cast<GLfloat>(-(modelview[4]*modelview[12] + modelview[5]*modelview[13] + modelview[6]*modelview[14])),
        static_cast<GLfloat>(-(modelview[8]*modelview[12] + modelview[9]*modelview[13] + modelview[10]*modelview[14]))
    );
    
    // Get enemy direction
    Vector3 toEnemy = position - cameraPos;
    float length = sqrt(toEnemy.x*toEnemy.x + toEnemy.y*toEnemy.y + toEnemy.z*toEnemy.z);
    if (length > 0) {
        toEnemy.x /= length;
        toEnemy.y /= length;
        toEnemy.z /= length;
    }
    
    // Check if enemy is facing camera
    float dotProduct = forward.x * toEnemy.x + forward.y * toEnemy.y + forward.z * toEnemy.z;
    if (dotProduct <= 0) {
        return;
    }
    
    // Project enemy position (above hat at Y +5.2)
    GLdouble winX, winY, winZ;
    gluProject(position.x, position.y + HEALTH_BAR_Y_OFFSET, position.z,
               modelview, projection, view,
               &winX, &winY, &winZ);
    
    // Save current matrix
    glPushMatrix();
    
    // Switch to projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    // Set orthographic projection
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glOrtho(0, viewport[2], 0, viewport[3], -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    
    float barWidth = 100.0f;
    float barHeight = 10.0f;
    
    // Draw red background
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2), static_cast<GLfloat>(winY));
    glVertex2f(static_cast<GLfloat>(winX + barWidth/2), static_cast<GLfloat>(winY));
    glVertex2f(static_cast<GLfloat>(winX + barWidth/2), static_cast<GLfloat>(winY + barHeight));
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2), static_cast<GLfloat>(winY + barHeight));
    glEnd();
    
    // Draw health
    float healthRatio = currentHealth / maxHealth;
    if(healthRatio > 0.3f) glColor3f(0.0f, 1.0f, 0.0f);
    else glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2), static_cast<GLfloat>(winY));
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2 + barWidth * healthRatio), static_cast<GLfloat>(winY));
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2 + barWidth * healthRatio), static_cast<GLfloat>(winY + barHeight));
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2), static_cast<GLfloat>(winY + barHeight));
    glEnd();
    
    // Draw black border
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2), static_cast<GLfloat>(winY));
    glVertex2f(static_cast<GLfloat>(winX + barWidth/2), static_cast<GLfloat>(winY));
    glVertex2f(static_cast<GLfloat>(winX + barWidth/2), static_cast<GLfloat>(winY + barHeight));
    glVertex2f(static_cast<GLfloat>(winX - barWidth/2), static_cast<GLfloat>(winY + barHeight));
    glEnd();
    
    // Restore matrix
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glLineWidth(1.0f);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
