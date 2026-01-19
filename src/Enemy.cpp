#include "Enemy.h"
#include <iostream>
#include <cmath>

Enemy::Enemy(Vector3 ipos, Color icol) : position(ipos), yaw(0.0f), currentHealth(5.0f), maxHealth(5.0f), alive(true) {
    // Main body spheres (white)
    Color white(1.0f, 1.0f, 1.0f);
    bodyLower = new Sphere(ipos + Vector3(0.0f, 1.0f, 0.0f), 3.6f, white);  // diameter = radius * 2
    head = new Sphere(ipos + Vector3(0.0f, 3.0f, 0.0f), 2.4f, white);

    // Stick arms (brown) - use constructor with axis to angle outward
    Color brown(0.4f, 0.25f, 0.1f);
    leftArm = new Cylinder(ipos + Vector3(-1.8f, 1.5f, 0.0f), 1.5f, 0.16f,
                           Vector3(-1.0f, 0.2f, 0.0f), 0.0f);  // Point left and slightly up
    leftArm->setColor(brown);
    rightArm = new Cylinder(ipos + Vector3(1.8f, 1.5f, 0.0f), 1.5f, 0.16f,
                            Vector3(1.0f, 0.2f, 0.0f), 0.0f);   // Point right and slightly up
    rightArm->setColor(brown);

    // Carrot nose (orange) - using Cylinder since Cone doesn't support rotation
    Color orange(1.0f, 0.5f, 0.0f);
    // Use Cylinder constructor with axis parameter to point forward (Z direction)
    nose = new Cylinder(ipos + Vector3(0.0f, 3.0f, 1.2f), 0.5f, 0.16f, Vector3(0.0f, 0.0f, 1.0f), 0.0f);
    nose->setColor(orange);

    // Coal buttons (black)
    Color black(0.1f, 0.1f, 0.1f);
    button1 = new Sphere(ipos + Vector3(0.0f, 1.8f, 1.8f), 0.24f, black);
    button2 = new Sphere(ipos + Vector3(0.0f, 1.3f, 1.8f), 0.24f, black);
    button3 = new Sphere(ipos + Vector3(0.0f, 0.8f, 1.8f), 0.24f, black);

    // Hat (black)
    hatBrim = new Cylinder(ipos + Vector3(0.0f, 4.2f, 0.0f), 0.1f, 2.0f, black);  // flat disk
    hatTop = new Cylinder(ipos + Vector3(0.0f, 4.6f, 0.0f), 0.8f, 1.2f, black);   // tall cylinder

    // Eyes (black)
    leftEye = new Sphere(ipos + Vector3(-0.3f, 3.3f, 1.1f), 0.2f, black);
    rightEye = new Sphere(ipos + Vector3(0.3f, 3.3f, 1.1f), 0.2f, black);
}

Enemy::Enemy(Vector3 ipos, Color icolHead, Color icolBody) : position(ipos), yaw(0.0f), currentHealth(5.0f), maxHealth(5.0f), alive(true) {
    // Main body spheres (white - snowman should always be white)
    Color white(1.0f, 1.0f, 1.0f);
    bodyLower = new Sphere(ipos + Vector3(0.0f, 1.0f, 0.0f), 3.6f, white);
    head = new Sphere(ipos + Vector3(0.0f, 3.0f, 0.0f), 2.4f, white);

    // Stick arms (brown)
    Color brown(0.4f, 0.25f, 0.1f);
    leftArm = new Cylinder(ipos + Vector3(-1.8f, 1.5f, 0.0f), 1.5f, 0.16f,
                           Vector3(-1.0f, 0.2f, 0.0f), 0.0f);
    leftArm->setColor(brown);
    rightArm = new Cylinder(ipos + Vector3(1.8f, 1.5f, 0.0f), 1.5f, 0.16f,
                            Vector3(1.0f, 0.2f, 0.0f), 0.0f);
    rightArm->setColor(brown);

    // Carrot nose (orange)
    Color orange(1.0f, 0.5f, 0.0f);
    nose = new Cylinder(ipos + Vector3(0.0f, 3.0f, 1.2f), 0.5f, 0.16f, Vector3(0.0f, 0.0f, 1.0f), 0.0f);
    nose->setColor(orange);

    // Coal buttons (black)
    Color black(0.1f, 0.1f, 0.1f);
    button1 = new Sphere(ipos + Vector3(0.0f, 1.8f, 1.8f), 0.24f, black);
    button2 = new Sphere(ipos + Vector3(0.0f, 1.3f, 1.8f), 0.24f, black);
    button3 = new Sphere(ipos + Vector3(0.0f, 0.8f, 1.8f), 0.24f, black);

    // Hat (black)
    hatBrim = new Cylinder(ipos + Vector3(0.0f, 4.2f, 0.0f), 0.1f, 2.0f, black);
    hatTop = new Cylinder(ipos + Vector3(0.0f, 4.6f, 0.0f), 0.8f, 1.2f, black);

    // Eyes (black)
    leftEye = new Sphere(ipos + Vector3(-0.3f, 3.3f, 1.1f), 0.2f, black);
    rightEye = new Sphere(ipos + Vector3(0.3f, 3.3f, 1.1f), 0.2f, black);
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
    bodyLower->setPosition(position + Vector3(0.0f, 1.0f, 0.0f));
    head->setPosition(position + Vector3(0.0f, 3.0f, 0.0f));

    // Arms (rotate positions and axes)
    Vector3 leftArmOffset = rotateOffset(-1.8f, 0.0f);
    leftArm->setPosition(position + leftArmOffset + Vector3(0.0f, 1.5f, 0.0f));

    Vector3 rightArmOffset = rotateOffset(1.8f, 0.0f);
    rightArm->setPosition(position + rightArmOffset + Vector3(0.0f, 1.5f, 0.0f));

    // Nose (rotate position)
    Vector3 noseOffset = rotateOffset(0.0f, 1.2f);
    nose->setPosition(position + noseOffset + Vector3(0.0f, 3.0f, 0.0f));

    // Buttons (rotate positions)
    Vector3 buttonOffset = rotateOffset(0.0f, 1.8f);
    button1->setPosition(position + buttonOffset + Vector3(0.0f, 1.8f, 0.0f));
    button2->setPosition(position + buttonOffset + Vector3(0.0f, 1.3f, 0.0f));
    button3->setPosition(position + buttonOffset + Vector3(0.0f, 0.8f, 0.0f));

    // Hat (centered, no XZ offset)
    hatBrim->setPosition(position + Vector3(0.0f, 4.2f, 0.0f));
    hatTop->setPosition(position + Vector3(0.0f, 4.6f, 0.0f));

    // Eyes (rotate positions)
    Vector3 leftEyeOffset = rotateOffset(-0.3f, 1.1f);
    leftEye->setPosition(position + leftEyeOffset + Vector3(0.0f, 3.3f, 0.0f));

    Vector3 rightEyeOffset = rotateOffset(0.3f, 1.1f);
    rightEye->setPosition(position + rightEyeOffset + Vector3(0.0f, 3.3f, 0.0f));
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
    gluProject(position.x, position.y + 5.2f, position.z,
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
