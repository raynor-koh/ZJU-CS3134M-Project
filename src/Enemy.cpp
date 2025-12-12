#include "Enemy.h"
#include <iostream>

Enemy::Enemy(Vector3 ipos, Color icol) : position(ipos), currentHealth(5.0f), maxHealth(5.0f), alive(true) {
    head = new Sphere(ipos+Vector3(0.0f, 2.5f, 0.0f), 1.0f, icol);
    body = new Cylinder(ipos+Vector3(0.0f, 0.8f, 0.0f), 1.6f, 2.0f, icol);
}

Enemy::Enemy(Vector3 ipos, Color icolHead, Color icolBody) : position(ipos), currentHealth(5.0f), maxHealth(5.0f), alive(true) {
    head = new Sphere(ipos+Vector3(0.0f, 2.5f, 0.0f), 1.0f, icolHead);
    body = new Cylinder(ipos+Vector3(0.0f, 0.8f, 0.0f), 1.6f, 2.0f, icolBody);
}

void Enemy::draw() {
    head->draw();
    body->draw();
    drawHealthBar();
}

void Enemy::setColor(Color icolor, PartType part) {
    switch (part) {
        case HEAD:
            head->setColor(icolor);
            break;
        case BODY:
            body->setColor(icolor);
            break;
    }
}

void Enemy::updatePos() {
    body->setPosition(position + Vector3(0.0f, 1.0f, 0.0f));
    head->setPosition(position + Vector3(0.0f, 2.5f, 0.0f));
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
    
    // Project enemy position
    GLdouble winX, winY, winZ;
    gluProject(position.x, position.y + 3.0f, position.z, 
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
