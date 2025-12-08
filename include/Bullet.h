#pragma once
#include "GameObject.h"

class Bullet {
public:
    Bullet(const Vector3& position, const Vector3& direction, float speed = 50.0f, float damage = 25.0f);

    void update(float deltaTime);
    void draw() const;

    // Getters
    Vector3 getPosition() const { return position; }
    float getRadius() const { return radius; }
    float getDamage() const { return damage; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }

    // Check if bullet has traveled too far or lived too long
    bool shouldRemove() const;

private:
    Vector3 position;
    Vector3 velocity;  // Direction * speed
    float speed;
    float radius;
    float damage;      // Damage dealt to targets
    float lifetime;    // Time since bullet was created
    float maxLifetime; // Maximum time bullet can exist
    bool active;
};
