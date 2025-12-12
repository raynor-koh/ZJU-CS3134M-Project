#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <algorithm>
#include "Player.h"
#include "CollisionDetector.h"

void Player::init() {
    head = new Sphere(Vector3(0.0f, 1.0f, 0.0f), 0.5f, Color(0.0f, 0.0f, 1.0f));
    body = new Cylinder(Vector3(0.0f, 0.5f, 0.0f), 1.0f, 0.7f, Color(0.0f, 0.0f, 1.0f));
    verticalVelocity = 0.0f;
    groundLevel = 0.0f;
    isOnGround = true;
    testDraw = true;
    return;
}

void Player::draw() {
    if(!visible) return;
    head->draw();
    body->draw();
    drawDirection();
    if(testDraw) drawCoordinateAxes();
}

void Player::drawCoordinateAxes() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    float x = position.x;
    float y = position.y;
    float z = position.z;
    
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x, y, z);
    glVertex3f(x + 2.0f, y, z);
     
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x, y, z);
    glVertex3f(x, y + 2.0f, z);
    
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x, y, z);
    glVertex3f(x, y, z + 2.0f);
    
    glEnd();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void Player::setColor(Color icolor, PartType part) {
    switch (part) {
        case HEAD:
            head->setColor(icolor);
            break;
        case BODY:
            body->setColor(icolor);
            break;
    }
}

void Player::updatePos() {
    body->setPosition(position + Vector3(0.0f, 0.35f, 0.0f));
    head->setPosition(position + Vector3(0.0f, 0.8f, 0.0f));
}

void Player::moveAbsolute(float dx, float dy, float dz) {
    position.x += dx;
    position.y += dy;
    position.z += dz;
    updatePos();
}

void Player::drawDirection() {
    glPushMatrix();

    glTranslatef(position.x, position.y , position.z);

    float angle = atan2(visionDirection.z, visionDirection.x) * 180.0f / float(M_PI);
    glRotatef(-angle - 90.0f, 0.0f, 1.0f, 0.0f);

    glColor3f(1.0f, 1.0f, 0.0f);

    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.01f, 0.4f);
        glVertex3f(-0.2f, 0.01f, 0.0f);
        glVertex3f(0.2f, 0.01f, 0.0f);
    glEnd();
    
    glPopMatrix();
}

bool Player::checkCollision(const std::vector<std::shared_ptr<Shape>>& objects) {
    for (size_t i = 0; i < objects.size(); ++i) {
        auto object = objects[i];
        if (!object) continue;
        Shape* bodyShape = dynamic_cast<Shape*>(body);
        Shape* headShape = dynamic_cast<Shape*>(head);
        if (CollisionDetector::checkCollision(bodyShape, object.get()) ||
            CollisionDetector::checkCollision(headShape, object.get())) {
            return true;
        }
    }
    return false;
}

bool Player::checkCollision(const std::vector<Enemy*>& enemies) {
    for (size_t i = 0; i < enemies.size(); ++i) {
        auto enemy = enemies[i];
        if (!enemy) continue;
        Shape* bodyShape = dynamic_cast<Shape*>(body);
        Shape* headShape = dynamic_cast<Shape*>(head);
        if (CollisionDetector::checkCollision(bodyShape, enemy->getHeadShape()) ||
            CollisionDetector::checkCollision(bodyShape, enemy->getBodyShape()) ||
            CollisionDetector::checkCollision(headShape, enemy->getHeadShape()) ||
            CollisionDetector::checkCollision(headShape, enemy->getBodyShape())) {
            return true;
        }
    }
    return false;
}

bool Player::checkSceneCollision() {
    if (!scene) return false;
    float groundSize = scene->getGroundSize();
    float radius = max(body->getSize().x/2.0f, head->getRadius());
    float boundaryMin = -groundSize + radius;
    float boundaryMax = groundSize - radius;

    if (position.x < boundaryMin || position.x > boundaryMax || position.z < boundaryMin || position.z > boundaryMax) {
        return true; // Collision with boundary walls
    }

    return checkCollision(scene->getObjects()) || checkCollision(scene->getEnemies());
}

void Player::jump() {
    if (isOnGround) {
        verticalVelocity = JUMP_VELOCITY;
        isOnGround = false;
    }
}

void Player::update(float deltaTime) {
    // Apply gravity
    if (!isOnGround) {
        verticalVelocity -= GRAVITY * deltaTime;
    }

    // Update Y position based on vertical velocity
    position.y += verticalVelocity * deltaTime;

    // Ground collision
    if (position.y <= groundLevel) {
        position.y = groundLevel;
        verticalVelocity = 0.0f;
        isOnGround = true;
    } else {
        isOnGround = false;
    }
    updatePos();
}