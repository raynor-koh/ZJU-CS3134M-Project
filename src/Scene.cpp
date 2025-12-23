#include "Scene.h"
#include "Bullet.h"
#include "Target.h"
#include "Shapes.h"
#include "Enemy.h"
#include "CollisionDetector.h"
#include "Lighting.h"
#include <GL/glut.h>
#include <cmath>

Scene::Scene()
    : groundSize(50.0f), groundColor(0.2f, 0.6f, 0.2f), wallHeight(5.0f), wallThickness(1.0f), lighting(nullptr) {
}

Scene::~Scene() {
    clearGameObjects();
    clearBullets();
    // Note: targets are managed separately and should be deleted by the caller
    targets.clear();
}

void Scene::initialize() {
    // Add default objects to the scene
    // addGameObject(new GameObject(Vector3(0.0f, 1.0f, 0.0f), Vector3(2.0f, 2.0f, 2.0f), Color(0.8f, 0.3f, 0.3f)));
    // addGameObject(new GameObject(Vector3(5.0f, 0.5f, -3.0f), Vector3(1.0f, 1.0f, 1.0f), Color(0.3f, 0.3f, 0.8f)));
    // addGameObject(new GameObject(Vector3(-4.0f, 1.5f, 2.0f), Vector3(3.0f, 3.0f, 3.0f), Color(0.8f, 0.8f, 0.3f)));
    // addGameObject(new GameObject(Vector3(8.0f, 1.0f, 5.0f), Vector3(2.0f, 2.0f, 2.0f), Color(0.5f, 0.2f, 0.6f)));
    // addGameObject(new GameObject(Vector3(-7.0f, 0.75f, -6.0f), Vector3(1.5f, 1.5f, 1.5f), Color(0.3f, 0.7f, 0.5f)));
    // addGameObject(new GameObject(Vector3(3.0f, 2.0f, 8.0f), Vector3(4.0f, 4.0f, 4.0f), Color(0.9f, 0.5f, 0.2f)));
    addTexture(new Texture("resources/textures/WoodCap.bmp"));
    addEnemy(new Enemy(Vector3(-5.0f, 0.0f, -5.0f), Color(0.7f, 1.0f, 0.3f), Color(0.1f, 0.3f, 0.5f)));
    addShape(std::make_shared<Cylinder>(Vector3(0.0f, 0.5f, 3.0f), 1.0f, 1.0f, Color(0.1f, 1.0f, 1.0f)));  // Y = height/2 = 0.5
    addShape(std::make_shared<Cylinder>(Vector3(0.0f, 0.5f, 5.0f), 1.0f, 1.0f, Color(0.4f, 0.1f, 0.9f)));  // Already correct
    addShape(std::make_shared<Sphere>(Vector3(5.0f, 0.2f, 4.2f), 0.4f, Color(0.8f, 0.2f, 0.6f)));  // Y = diameter/2 = 0.2
    addShape(std::make_shared<Cube>(Vector3(0.0f, 0.5f, 0.0f), Vector3(2.0f, 1.0f, 2.0f), Color(0.8f, 0.3f, 0.3f)));  // Y = size.y/2 = 0.5
    addShape(std::make_shared<Cube>(Vector3(5.0f, 0.5f, -3.0f), Vector3(1.0f, 1.0f, 1.0f), Color(0.3f, 0.3f, 0.8f)));  // Already correct
    addShape(std::make_shared<Cube>(Vector3(-4.0f, 1.5f, 2.0f), Vector3(1.0f, 3.0f, 5.0f), Color(0.8f, 0.8f, 0.3f)));  // Already correct
    addShape(std::make_shared<Cube>(Vector3(10.0f, 1.0f, 5.0f), Vector3(2.0f, 2.0f, 2.0f), Color(0.5f, 0.2f, 0.6f)));  // Already correct
    addShape(std::make_shared<Cube>(Vector3(-7.0f, 0.75f, -6.0f), Vector3(1.0f, 1.5f, 1.5f), Color(0.3f, 0.7f, 0.5f)));  // Already correct
    addShape(std::make_shared<Cube>(Vector3(3.0f, 4.0f, 10.0f), Vector3(4.0f, 8.0f, 4.0f), Color(0.9f, 0.5f, 0.2f)));  // Y = size.y/2 = 4.0
}

void Scene::draw() const {
    // NOTE: Lighting is applied in display() AFTER camera view is set
    // This allows for proper headlight mode

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
    
    for (const auto& shape : objects) {
        shape->draw();
    }
    
    // Draw enemies
    for (const auto& enemy : enemies) {
        enemy->draw();
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

void Scene::addShape(std::shared_ptr<Shape> shape) {
    objects.push_back(shape);
}

void Scene::addTexture(Texture* texture) {
    textures.push_back(texture);
}

void Scene::addEnemy(Enemy* enemy) {
    enemies.push_back(enemy);
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

// Get the highest ground level at a given XZ position (for landing on boxes)
float Scene::getGroundHeightAt(float x, float z, float radius, float currentY) const {
    float highestGround = 0.0f; // Start with ground level
    const float SKIN_WIDTH = 0.01f; // Small epsilon for stable landing

    // Check all shapes to find the highest one the player can land on
    for (const auto& shape : objects) {
        if (!shape) continue;

        Vector3 shapePos = shape->getPosition();
        Vector3 shapeSize = shape->getSize();

        // Calculate AABB of the shape
        float boxMinX = shapePos.x - shapeSize.x / 2.0f;
        float boxMaxX = shapePos.x + shapeSize.x / 2.0f;
        float boxMinZ = shapePos.z - shapeSize.z / 2.0f;
        float boxMaxZ = shapePos.z + shapeSize.z / 2.0f;
        float boxMaxY = shapePos.y + shapeSize.y / 2.0f;
        float boxMinY = shapePos.y - shapeSize.y / 2.0f;

        // Check if player's XZ position overlaps with this box (with radius)
        float closestX = (x < boxMinX) ? boxMinX : (x > boxMaxX) ? boxMaxX : x;
        float closestZ = (z < boxMinZ) ? boxMinZ : (z > boxMaxZ) ? boxMaxZ : z;

        float distX = x - closestX;
        float distZ = z - closestZ;
        float distanceSquared = distX * distX + distZ * distZ;

        // If player is above this box and overlaps in XZ
        if (distanceSquared <= (radius * radius) && currentY >= boxMinY) {
            // This box's top surface is a potential landing spot
            if (boxMaxY > highestGround && boxMaxY <= currentY + SKIN_WIDTH) {
                highestGround = boxMaxY;
            }
        }
    }

    return highestGround;
}

// Check vertical collision - returns true if landing on something, sets outGroundY
bool Scene::checkVerticalCollision(float x, float y, float z, float radius, float height, float& outGroundY) const {
    const float SKIN_WIDTH = 0.01f;
    float playerBottom = y;
    float playerTop = y + height;

    outGroundY = 0.0f; // Default ground level
    bool foundGround = false;

    // Check all shapes for vertical collision
    for (const auto& shape : objects) {
        if (!shape) continue;

        Vector3 shapePos = shape->getPosition();
        Vector3 shapeSize = shape->getSize();

        // Calculate AABB of the shape
        float boxMinX = shapePos.x - shapeSize.x / 2.0f;
        float boxMaxX = shapePos.x + shapeSize.x / 2.0f;
        float boxMinZ = shapePos.z - shapeSize.z / 2.0f;
        float boxMaxZ = shapePos.z + shapeSize.z / 2.0f;
        float boxMaxY = shapePos.y + shapeSize.y / 2.0f;
        float boxMinY = shapePos.y - shapeSize.y / 2.0f;

        // Check horizontal overlap first (player XZ vs box XZ)
        float closestX = (x < boxMinX) ? boxMinX : (x > boxMaxX) ? boxMaxX : x;
        float closestZ = (z < boxMinZ) ? boxMinZ : (z > boxMaxZ) ? boxMaxZ : z;

        float distX = x - closestX;
        float distZ = z - closestZ;
        float distanceSquared = distX * distX + distZ * distZ;

        // If player overlaps horizontally with this box
        if (distanceSquared <= (radius * radius)) {
            // Check if landing on top
            if (playerBottom <= boxMaxY + SKIN_WIDTH && playerBottom >= boxMinY) {
                if (boxMaxY > outGroundY) {
                    outGroundY = boxMaxY;
                    foundGround = true;
                }
            }
            // Check if hitting ceiling
            else if (playerTop >= boxMinY && playerTop <= boxMaxY + SKIN_WIDTH) {
                // Hitting bottom of box while jumping up - not handled here, just note overlap
            }
        }
    }

    return foundGround;
}

// Check collision with all objects and boundary walls (horizontal XZ collision only)
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

    // NOTE: Shape collision checking removed from 2D version
    // Use the Y-aware checkCollision(x, y, z, radius, height) instead
    // This ensures proper collision detection when standing on boxes

    return false; // No collision
}

// Y-aware collision detection - checks if player at given height would collide with boxes
bool Scene::checkCollision(float x, float y, float z, float radius, float height) const {
    const float MARGIN = 0.1f; // Small margin to allow walking off edges

    // Check boundary walls collision
    float boundaryMin = -groundSize + radius;
    float boundaryMax = groundSize - radius;

    if (x < boundaryMin || x > boundaryMax || z < boundaryMin || z > boundaryMax) {
        return true; // Collision with boundary walls
    }

    // Check collision with all game objects (these use old 2D collision)
    for (const auto& obj : gameObjects) {
        if (obj->checkAABBCollision(x, z, radius)) {
            return true; // Collision with object
        }
    }

    // Check collision with shape objects - Y-aware
    for (const auto& shape : objects) {
        if (!shape) continue;

        Vector3 shapePos = shape->getPosition();
        Vector3 shapeSize = shape->getSize();

        // Calculate box bounds
        float boxMinX = shapePos.x - shapeSize.x / 2.0f;
        float boxMaxX = shapePos.x + shapeSize.x / 2.0f;
        float boxMinZ = shapePos.z - shapeSize.z / 2.0f;
        float boxMaxZ = shapePos.z + shapeSize.z / 2.0f;
        float boxMinY = shapePos.y - shapeSize.y / 2.0f;
        float boxMaxY = shapePos.y + shapeSize.y / 2.0f;

        // Player bounds
        float playerBottom = y;
        float playerTop = y + height;

        // CRITICAL: If player's feet are AT or ABOVE the box top, they're standing on it
        // Allow movement (no horizontal collision with this box)
        if (playerBottom >= boxMaxY - MARGIN) {
            continue; // Standing on top of box or above it, allow movement
        }

        // Player is below box top - check if they're actually inside the box vertically
        if (playerTop > boxMinY) {
            // Player vertically intersects with box, check XZ collision
            // Find closest point on box to player position in XZ plane
            float closestX = (x < boxMinX) ? boxMinX : (x > boxMaxX) ? boxMaxX : x;
            float closestZ = (z < boxMinZ) ? boxMinZ : (z > boxMaxZ) ? boxMaxZ : z;

            float distX = x - closestX;
            float distZ = z - closestZ;
            float distanceSquared = distX * distX + distZ * distZ;

            if (distanceSquared <= (radius * radius)) {
                return true; // Collision with shape sides
            }
        }
    }

    return false; // No collision
}

// Update scene (bullets, physics, etc.)
void Scene::update(float deltaTime) {
    for (auto& bullet : bullets) bullet->move(deltaTime);
    checkBulletCollisions();
    for (auto& bullet : bullets) {
        if(bullet->shouldRemove()) {
            delete bullet;
            bullets.erase(std::remove(bullets.begin(), bullets.end(), bullet), bullets.end());
        }
    }
}

void Scene::checkBulletCollisions() {
    for (auto& bullet : bullets) {
        if(!bullet->isActive()) continue;

        Vector3 bulletPos = bullet->getPosition();

        // Check wall collisions
        if (bulletPos.x < -groundSize || bulletPos.x > groundSize ||
            bulletPos.z < -groundSize || bulletPos.z > groundSize ||
            bulletPos.y < 0.0f || bulletPos.y > wallHeight) {
            bullet->deactivate();
            continue;
        }

        // Check Target collisions
        for (auto& target : targets) {
            if (!target->isAlive()) continue;
            if (target->checkAABBCollision(bulletPos.x, bulletPos.z, 0.1f)) {
                // Check Y coordinate (height) as well
                Vector3 targetPos = target->getPosition();
                Vector3 targetSize = target->getSize();
                if (bulletPos.y >= targetPos.y - targetSize.y &&
                    bulletPos.y <= targetPos.y + targetSize.y) {
                    bullet->deactivate();
                    target->takeDamage(bullet->getDamage());
                    break;
                }
            }
        }

        if(!bullet->isActive()) continue;

        // Check Enemy collisions
        for (auto& enemy : enemies) {
            if (CollisionDetector::checkCollision(bullet->getShape(), enemy->getHeadShape()) ||
                CollisionDetector::checkCollision(bullet->getShape(), enemy->getBodyShape())) {
                bullet->deactivate();
                enemy->takeDamage(bullet->getDamage());
                if(enemy->isAlive() == false) {
                    delete enemy;
                    enemies.erase(std::remove(enemies.begin(), enemies.end(), enemy), enemies.end());
                }
                break;
            }
        }

        if(!bullet->isActive()) continue;

        // Check Shape collisions (cubes, spheres, cylinders)
        for (auto& obj : objects) {
            if (CollisionDetector::checkCollision(bullet->getShape(), obj.get())) {
                bullet->deactivate();
                break;
            }
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

// Calculate scene bounding sphere for camera zoom-to-fit
void Scene::calculateSceneBounds(Vector3& center, float& radius) const {
    // Initialize AABB to invalid range
    // Using minBounds/maxBounds to avoid Windows min/max macro conflicts
    Vector3 minBounds(1000000.0f, 1000000.0f, 1000000.0f);
    Vector3 maxBounds(-1000000.0f, -1000000.0f, -1000000.0f);

    bool foundAnyObject = false;

    // Include all Shape objects (cubes, spheres, cylinders, etc.)
    for (const auto& obj : objects) {
        Vector3 objPos = obj->getPosition();
        Vector3 objSize = obj->getSize();

        // Compute AABB of this object (assuming size is full extent, not half)
        Vector3 objMin(objPos.x - objSize.x / 2.0f, objPos.y - objSize.y / 2.0f, objPos.z - objSize.z / 2.0f);
        Vector3 objMax(objPos.x + objSize.x / 2.0f, objPos.y + objSize.y / 2.0f, objPos.z + objSize.z / 2.0f);

        // Expand scene AABB
        minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
        minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
        minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

        maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
        maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
        maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

        foundAnyObject = true;
    }

    // Include enemies (use body and head shapes for bounds)
    for (const auto& enemy : enemies) {
        // Get bounds from body shape
        Shape* bodyShape = enemy->getBodyShape();
        if (bodyShape) {
            Vector3 bodyPos = bodyShape->getPosition();
            Vector3 bodySize = bodyShape->getSize();

            Vector3 objMin(bodyPos.x - bodySize.x / 2.0f, bodyPos.y - bodySize.y / 2.0f, bodyPos.z - bodySize.z / 2.0f);
            Vector3 objMax(bodyPos.x + bodySize.x / 2.0f, bodyPos.y + bodySize.y / 2.0f, bodyPos.z + bodySize.z / 2.0f);

            minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
            minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
            minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

            maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
            maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
            maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

            foundAnyObject = true;
        }

        // Get bounds from head shape
        Shape* headShape = enemy->getHeadShape();
        if (headShape) {
            Vector3 headPos = headShape->getPosition();
            Vector3 headSize = headShape->getSize();

            Vector3 objMin(headPos.x - headSize.x / 2.0f, headPos.y - headSize.y / 2.0f, headPos.z - headSize.z / 2.0f);
            Vector3 objMax(headPos.x + headSize.x / 2.0f, headPos.y + headSize.y / 2.0f, headPos.z + headSize.z / 2.0f);

            minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
            minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
            minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

            maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
            maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
            maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

            foundAnyObject = true;
        }
    }

    // Include targets
    for (const auto& target : targets) {
        Vector3 targetPos = target->getPosition();
        Vector3 targetSize = target->getSize();

        Vector3 objMin(targetPos.x - targetSize.x / 2.0f, targetPos.y - targetSize.y / 2.0f, targetPos.z - targetSize.z / 2.0f);
        Vector3 objMax(targetPos.x + targetSize.x / 2.0f, targetPos.y + targetSize.y / 2.0f, targetPos.z + targetSize.z / 2.0f);

        minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
        minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
        minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

        maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
        maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
        maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

        foundAnyObject = true;
    }

    // If no objects found, use default scene bounds (ground plane)
    if (!foundAnyObject) {
        minBounds = Vector3(-groundSize, 0.0f, -groundSize);
        maxBounds = Vector3(groundSize, wallHeight, groundSize);
    }

    // Calculate center of AABB
    center.x = (minBounds.x + maxBounds.x) / 2.0f;
    center.y = (minBounds.y + maxBounds.y) / 2.0f;
    center.z = (minBounds.z + maxBounds.z) / 2.0f;

    // Calculate radius as half the diagonal of AABB
    float dx = maxBounds.x - minBounds.x;
    float dy = maxBounds.y - minBounds.y;
    float dz = maxBounds.z - minBounds.z;
    radius = sqrt(dx * dx + dy * dy + dz * dz) / 2.0f;

    // Ensure minimum radius
    if (radius < 1.0f) {
        radius = 10.0f; // Default scene radius if very small
    }
}
