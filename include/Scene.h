#pragma once
#include "GameObject.h"
#include <vector>

class Scene {
public:
    Scene();
    ~Scene();

    void initialize();
    void draw() const;
    void addGameObject(GameObject* obj);
    void clearGameObjects();

    void setGroundSize(float size) { groundSize = size; }
    void setGroundColor(const Color& color) { groundColor = color; }

    // Collision detection
    bool checkCollision(float x, float z, float radius) const;
    float getGroundSize() const { return groundSize; }

private:
    void drawGround() const;
    void drawBoundaryWalls() const;

    std::vector<GameObject*> gameObjects;
    float groundSize;
    Color groundColor;
    float wallHeight;
    float wallThickness;
};
