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
    // Initialize health
    maxHealth = DEFAULT_MAX_HEALTH;
    currentHealth = maxHealth;
    return;
}

void Player::draw() {
    // Always draw health bar (it's 2D overlay, independent of visibility)
    drawHealthBar();

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

    const float PLAYER_HEIGHT = 1.4f; // Match the height used in update()
    const float PLAYER_RADIUS = 0.35f; // Match the radius used in update()

    // Use the Y-aware collision check from Scene
    // This properly handles standing on top of boxes vs colliding with sides
    return scene->checkCollision(position.x, position.y, position.z, PLAYER_RADIUS, PLAYER_HEIGHT);
}

void Player::jump() {
    if (isOnGround) {
        verticalVelocity = JUMP_VELOCITY;
        isOnGround = false;
    }
}

void Player::update(float deltaTime) {
    const float SKIN_WIDTH = 0.01f; // Small epsilon for stable landing
    const float PLAYER_HEIGHT = 1.4f; // Player total height (body + head)
    const float PLAYER_RADIUS = 0.35f; // Player collision radius

    // Apply gravity
    if (!isOnGround) {
        verticalVelocity -= GRAVITY * deltaTime;
    }

    // Store old position
    float oldY = position.y;

    // Update Y position based on vertical velocity (axis-separated: Y first)
    position.y += verticalVelocity * deltaTime;

    // Check for vertical collision with scene objects
    float groundY = 0.0f; // Default ground level
    bool foundGround = false;

    if (scene) {
        // Player position is at feet level, check vertical collision
        float playerBottom = position.y;

        // Check vertical collision with boxes
        scene->checkVerticalCollision(position.x, playerBottom, position.z, PLAYER_RADIUS, PLAYER_HEIGHT, groundY);

        // Landing detection: if moving downward and we're at or below a surface
        if (verticalVelocity <= 0.0f && playerBottom <= groundY + SKIN_WIDTH) {
            // Snap to surface with skin width for stable landing
            position.y = groundY + SKIN_WIDTH;
            verticalVelocity = 0.0f;
            isOnGround = true;
            foundGround = true;
        }
        // Ceiling collision: if moving upward and hit ceiling
        else if (verticalVelocity > 0.0f) {
            float playerTop = position.y + PLAYER_HEIGHT;
            // Check if head hit something (only trigger if actually found a box)
            float ceilingY = 0.0f;
            if (scene->checkVerticalCollision(position.x, playerTop - 0.1f, position.z, PLAYER_RADIUS, 0.1f, ceilingY) && ceilingY > 0.0f) {
                if (playerTop >= ceilingY - SKIN_WIDTH) {
                    // Hit ceiling, stop upward movement
                    verticalVelocity = 0.0f;
                    position.y = oldY; // Revert to previous position
                }
            }
        }
    }

    // Fallback to default ground level if no box found
    if (!foundGround) {
        if (position.y <= 0.0f + SKIN_WIDTH) {
            position.y = SKIN_WIDTH;
            verticalVelocity = 0.0f;
            isOnGround = true;
        } else {
            isOnGround = false;
        }
    }

    updatePos();
}

// Health system implementation
void Player::takeDamage(float damage) {
    if (currentHealth > 0.0f) {
        currentHealth -= damage;
        if (currentHealth < 0.0f) {
            currentHealth = 0.0f;
        }
        std::cout << "Player took " << damage << " damage! Health: " << currentHealth << "/" << maxHealth << std::endl;
    }
}

void Player::heal(float amount) {
    currentHealth += amount;
    if (currentHealth > maxHealth) {
        currentHealth = maxHealth;
    }
}

void Player::resetHealth() {
    currentHealth = maxHealth;
}

void Player::drawHealthBar() {
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

    // Health bar position and size (top-left corner)
    float barX = 20.0f;
    float barY = viewport[3] - 40.0f;  // 40 pixels from top
    float barWidth = 200.0f;
    float barHeight = 20.0f;

    // Draw background (dark gray)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();

    // Draw health (green to red based on health)
    float healthRatio = getHealthPercent();
    if (healthRatio > 0.5f) {
        glColor3f(0.0f, 1.0f, 0.0f);  // Green
    } else if (healthRatio > 0.25f) {
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
    } else {
        glColor3f(1.0f, 0.0f, 0.0f);  // Red
    }

    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth * healthRatio, barY);
    glVertex2f(barX + barWidth * healthRatio, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();

    // Draw border (black)
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(barX, barY);
    glVertex2f(barX + barWidth, barY);
    glVertex2f(barX + barWidth, barY + barHeight);
    glVertex2f(barX, barY + barHeight);
    glEnd();
    glLineWidth(1.0f);

    // Restore OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}