#pragma once
#include "GameObject.h"
#include <vector>

class Bullet; // Forward declaration
class Target; // Forward declaration

class Scene {
public:
    Scene();
    ~Scene();

    void initialize();
    void draw() const;
    void update(float deltaTime);
    void addGameObject(GameObject* obj);
    void clearGameObjects();

    void setGroundSize(float size) { groundSize = size; }
    void setGroundColor(const Color& color) { groundColor = color; }

    // Collision detection
    bool checkCollision(float x, float z, float radius) const;
    float getGroundSize() const { return groundSize; }

    // Bullet and target management
    void addTarget(Target* target);
    void fireBullet(const Vector3& position, const Vector3& direction);
    void clearBullets();

private:
    void drawGround() const;
    void drawBoundaryWalls() const;
    void updateBullets(float deltaTime);
    void checkBulletCollisions();

    std::vector<GameObject*> gameObjects;
    std::vector<Bullet*> bullets;
    std::vector<Target*> targets;
    float groundSize;
    Color groundColor;
    float wallHeight;
    float wallThickness;
};
