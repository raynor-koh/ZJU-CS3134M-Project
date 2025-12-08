#include "Scene.h"
#include "Bullet.h"
#include "Target.h"
#include <GL/glut.h>
#include <cmath>

Scene::Scene()
    : groundSize(50.0f), groundColor(0.2f, 0.6f, 0.2f), wallHeight(5.0f), wallThickness(1.0f) {
}

Scene::~Scene() {
    clearGameObjects();
    clearBullets();
    // Note: targets are managed separately and should be deleted by the caller
    targets.clear();
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
    drawBoundaryWalls();

    for (const auto& obj : gameObjects) {
        obj->draw();
    }

    // Draw targets
    for (const auto& target : targets) {
        target->draw();
    }

    // Draw bullets
    for (const auto& bullet : bullets) {
        bullet->draw();
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

void Scene::drawBoundaryWalls() const {
    glPushMatrix();
    glColor3f(0.4f, 0.3f, 0.2f); // Brown/tan color for walls

    // North wall (positive Z)
    glPushMatrix();
    glTranslatef(0.0f, wallHeight / 2.0f, groundSize);
    glScalef(groundSize * 2.0f + wallThickness * 2.0f, wallHeight, wallThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // South wall (negative Z)
    glPushMatrix();
    glTranslatef(0.0f, wallHeight / 2.0f, -groundSize);
    glScalef(groundSize * 2.0f + wallThickness * 2.0f, wallHeight, wallThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // East wall (positive X)
    glPushMatrix();
    glTranslatef(groundSize, wallHeight / 2.0f, 0.0f);
    glScalef(wallThickness, wallHeight, groundSize * 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // West wall (negative X)
    glPushMatrix();
    glTranslatef(-groundSize, wallHeight / 2.0f, 0.0f);
    glScalef(wallThickness, wallHeight, groundSize * 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

// Check collision with all objects and boundary walls
bool Scene::checkCollision(float x, float z, float radius) const {
    // Check boundary walls collision (simplified - treat as rectangle boundary)
    float boundaryMin = -groundSize + radius;
    float boundaryMax = groundSize - radius;

    if (x < boundaryMin || x > boundaryMax || z < boundaryMin || z > boundaryMax) {
        return true; // Collision with boundary walls
    }

    // Check collision with all game objects
    for (const auto& obj : gameObjects) {
        if (obj->checkAABBCollision(x, z, radius)) {
            return true; // Collision with object
        }
    }

    return false; // No collision
}

// Update scene (bullets, physics, etc.)
void Scene::update(float deltaTime) {
    updateBullets(deltaTime);
    checkBulletCollisions();

    // Remove dead targets
    for (auto it = targets.begin(); it != targets.end();) {
        if (!(*it)->isAlive()) {
            delete *it;
            it = targets.erase(it);
        } else {
            ++it;
        }
    }
}

// Add a target to the scene
void Scene::addTarget(Target* target) {
    targets.push_back(target);
}

// Fire a bullet from a position in a direction
void Scene::fireBullet(const Vector3& position, const Vector3& direction) {
    bullets.push_back(new Bullet(position, direction));
}

// Clear all bullets
void Scene::clearBullets() {
    for (auto& bullet : bullets) {
        delete bullet;
    }
    bullets.clear();
}

// Update all bullets (movement, lifetime)
void Scene::updateBullets(float deltaTime) {
    // Remove bullets that should be deleted
    for (auto it = bullets.begin(); it != bullets.end();) {
        (*it)->update(deltaTime);
        if ((*it)->shouldRemove()) {
            delete *it;
            it = bullets.erase(it);
        } else {
            ++it;
        }
    }
}

// Check for collisions between bullets and targets
void Scene::checkBulletCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet->isActive()) continue;

        Vector3 bulletPos = bullet->getPosition();
        float bulletRadius = bullet->getRadius();

        // Check collision with each target
        for (auto& target : targets) {
            Vector3 targetPos = target->getPosition();
            Vector3 targetSize = target->getSize();

            // Simple sphere-to-box collision detection
            // Find closest point on box to bullet sphere
            float closestX = std::max(targetPos.x - targetSize.x / 2.0f,
                                     std::min(bulletPos.x, targetPos.x + targetSize.x / 2.0f));
            float closestY = std::max(targetPos.y - targetSize.y / 2.0f,
                                     std::min(bulletPos.y, targetPos.y + targetSize.y / 2.0f));
            float closestZ = std::max(targetPos.z - targetSize.z / 2.0f,
                                     std::min(bulletPos.z, targetPos.z + targetSize.z / 2.0f));

            // Calculate distance
            float distX = bulletPos.x - closestX;
            float distY = bulletPos.y - closestY;
            float distZ = bulletPos.z - closestZ;
            float distanceSquared = distX * distX + distY * distY + distZ * distZ;

            // Check if collision occurred
            if (distanceSquared < (bulletRadius * bulletRadius)) {
                // Deal damage to the target
                target->takeDamage(bullet->getDamage());
                bullet->deactivate();
                break; // Bullet can only hit one target
            }
        }
    }
}
