#pragma once
#include "GameObject.h"

class Target : public GameObject {
public:
    Target(const Vector3& position, const Vector3& size, const Color& color, float maxHealth = 100.0f);

    // Target-specific functionality
    void takeDamage(float damage);
    bool isAlive() const { return currentHealth > 0.0f; }
    void reset();

    // Health accessors
    float getCurrentHealth() const { return currentHealth; }
    float getMaxHealth() const { return maxHealth; }
    float getHealthPercentage() const { return currentHealth / maxHealth; }

    // Override draw to show health bar
    void draw() const;

private:
    float maxHealth;
    float currentHealth;
    void drawHealthBar() const;
};
