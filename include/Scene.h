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

private:
    void drawGround() const;

    std::vector<GameObject*> gameObjects;
    float groundSize;
    Color groundColor;
};
