#include "Scene.h"
#include <GL/glut.h>

Scene::Scene()
    : groundSize(50.0f), groundColor(0.2f, 0.6f, 0.2f) {
}

Scene::~Scene() {
    clearGameObjects();
}

void Scene::initialize() {
    // Add default objects to the scene
    addGameObject(new GameObject(Vector3(0.0f, 1.0f, 0.0f), Vector3(2.0f, 2.0f, 2.0f), Color(0.8f, 0.3f, 0.3f)));
    addGameObject(new GameObject(Vector3(5.0f, 0.5f, -3.0f), Vector3(1.0f, 1.0f, 1.0f), Color(0.3f, 0.3f, 0.8f)));
    addGameObject(new GameObject(Vector3(-4.0f, 1.5f, 2.0f), Vector3(3.0f, 3.0f, 3.0f), Color(0.8f, 0.8f, 0.3f)));
    addGameObject(new GameObject(Vector3(8.0f, 1.0f, 5.0f), Vector3(2.0f, 2.0f, 2.0f), Color(0.5f, 0.2f, 0.6f)));
    addGameObject(new GameObject(Vector3(-7.0f, 0.75f, -6.0f), Vector3(1.5f, 1.5f, 1.5f), Color(0.3f, 0.7f, 0.5f)));
    addGameObject(new GameObject(Vector3(3.0f, 2.0f, 8.0f), Vector3(4.0f, 4.0f, 4.0f), Color(0.9f, 0.5f, 0.2f)));
}

void Scene::draw() const {
    drawGround();

    for (const auto& obj : gameObjects) {
        obj->draw();
    }
}

void Scene::addGameObject(GameObject* obj) {
    gameObjects.push_back(obj);
}

void Scene::clearGameObjects() {
    for (auto& obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
}

void Scene::drawGround() const {
    glPushMatrix();
    glColor3f(groundColor.r, groundColor.g, groundColor.b);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);

    glVertex3f(-groundSize, 0.0f, -groundSize);
    glVertex3f(groundSize, 0.0f, -groundSize);
    glVertex3f(groundSize, 0.0f, groundSize);
    glVertex3f(-groundSize, 0.0f, groundSize);

    glEnd();
    glPopMatrix();
}
