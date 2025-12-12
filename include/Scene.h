#pragma once
#include "GameObject.h"
#include <vector>
#include <memory>

class Bullet; // Forward declaration
class Target; // Forward declaration
class Shape;
class Enemy;
class Texture;

class Scene {
public:
    Scene();
    ~Scene();

    void initialize();
    void draw() const;
    void update(float deltaTime);
    void addGameObject(GameObject* obj);
    void clearGameObjects();
    void addShape(std::shared_ptr<Shape> shape);
    void addTexture(Texture* texture);
    void addEnemy(Enemy* enemy);

    void setGroundSize(float size) { groundSize = size; }
    void setGroundColor(const Color& color) { groundColor = color; }

    // Collision detection
    bool checkCollision(float x, float z, float radius) const;
    float getGroundSize() const { return groundSize; }

    // Bullet and target management
    void addTarget(Target* target);
    void fireBullet(const Vector3& position, const Vector3& direction);
    void clearBullets();

    const std::vector<std::shared_ptr<Shape>>& getObjects() const { return objects; }
    const std::vector<Enemy*>& getEnemies() const { return enemies; }

private:
    void drawGround() const;
    void drawBoundaryWalls() const;
    // void updateBullets(float deltaTime);
    void checkBulletCollisions();

    std::vector<GameObject*> gameObjects;
    std::vector<Bullet*> bullets;
    std::vector<Texture*> textures;
    std::vector<Target*> targets;
    std::vector<std::shared_ptr<Shape>> objects;
    std::vector<Enemy*> enemies;

    float groundSize;
    Color groundColor;
    float wallHeight;
    float wallThickness;
};
