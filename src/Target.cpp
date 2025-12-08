#include "Target.h"
#include <GL/glut.h>
#include <algorithm>

Target::Target(const Vector3& position, const Vector3& size, const Color& color, float maxHealth)
    : GameObject(position, size, color), maxHealth(maxHealth), currentHealth(maxHealth) {
    // Targets don't block player movement
    setCollisionType(CollisionType::NONE);
}

void Target::takeDamage(float damage) {
    currentHealth -= damage;
    if (currentHealth < 0.0f) {
        currentHealth = 0.0f;
    }
}

void Target::reset() {
    currentHealth = maxHealth;
}

void Target::draw() const {
    if (!isAlive()) {
        return; // Don't draw dead targets
    }

    glPushMatrix();
    glTranslatef(getPosition().x, getPosition().y, getPosition().z);

    // Color based on health percentage
    float healthPercent = getHealthPercentage();
    Color c = getColor();

    // Blend from original color to red as health decreases
    float r = c.r + (1.0f - c.r) * (1.0f - healthPercent);
    float g = c.g * healthPercent;
    float b = c.b * healthPercent;
    glColor3f(r, g, b);

    Vector3 s = getSize();
    glScalef(s.x, s.y, s.z);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw health bar above the target
    drawHealthBar();
}

void Target::drawHealthBar() const {
    if (!isAlive()) {
        return;
    }

    Vector3 pos = getPosition();
    Vector3 size = getSize();

    // Health bar position (above the target)
    float barWidth = size.x * 1.2f;
    float barHeight = 0.2f;
    float barY = pos.y + size.y / 2.0f + 0.5f; // Above the target

    glDisable(GL_LIGHTING); // Disable lighting for health bar
    glDisable(GL_DEPTH_TEST); // Disable depth testing to prevent z-fighting

    glPushMatrix();
    glTranslatef(pos.x, barY, pos.z);

    // Rotate to face camera (billboard effect - simplified, always faces forward)
    // For a proper billboard, you'd need camera direction

    // Background (red - missing health)
    glColor3f(0.3f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(-barWidth / 2.0f, 0.0f, 0.0f);
    glVertex3f(barWidth / 2.0f, 0.0f, 0.0f);
    glVertex3f(barWidth / 2.0f, barHeight, 0.0f);
    glVertex3f(-barWidth / 2.0f, barHeight, 0.0f);
    glEnd();

    // Foreground (green - current health)
    float healthPercent = getHealthPercentage();
    float healthWidth = barWidth * healthPercent;

    // Color transitions: green -> yellow -> red
    float r = (healthPercent < 0.5f) ? 1.0f : 2.0f * (1.0f - healthPercent);
    float g = (healthPercent > 0.5f) ? 1.0f : 2.0f * healthPercent;
    glColor3f(r, g, 0.0f);

    glBegin(GL_QUADS);
    glVertex3f(-barWidth / 2.0f, 0.0f, 0.0f);
    glVertex3f(-barWidth / 2.0f + healthWidth, 0.0f, 0.0f);
    glVertex3f(-barWidth / 2.0f + healthWidth, barHeight, 0.0f);
    glVertex3f(-barWidth / 2.0f, barHeight, 0.0f);
    glEnd();

    // Border (black outline)
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-barWidth / 2.0f, 0.0f, 0.0f);
    glVertex3f(barWidth / 2.0f, 0.0f, 0.0f);
    glVertex3f(barWidth / 2.0f, barHeight, 0.0f);
    glVertex3f(-barWidth / 2.0f, barHeight, 0.0f);
    glEnd();

    glPopMatrix();

    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
    glEnable(GL_LIGHTING); // Re-enable lighting
}
