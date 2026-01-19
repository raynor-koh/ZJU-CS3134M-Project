#pragma once
#include "GameObject.h"
#include "CollisionGrid.h"
#include "NavigationGrid.h"
#include <vector>
#include <memory>

class Bullet; // Forward declaration
class Target; // Forward declaration
class Shape;
class Enemy;
class Texture;
class Lighting; // Forward declaration

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
    bool checkCollision(float x, float y, float z, float radius, float height) const;
    float getGroundSize() const { return groundSize; }

    // Vertical collision detection for landing on boxes
    float getGroundHeightAt(float x, float z, float radius, float currentY) const;
    bool checkVerticalCollision(float x, float y, float z, float radius, float height, float& outGroundY) const;

    // Bullet and target management
    void addTarget(Target* target);
    void fireBullet(const Vector3& position, const Vector3& direction);
    void clearBullets();

    const std::vector<std::shared_ptr<Shape>>& getObjects() const { return objects; }
    const std::vector<Enemy*>& getEnemies() const { return enemies; }

    // Grid-based collision detection for enemies
    const CollisionGrid& getCollisionGrid() const { return collisionGrid; }
    void rebuildCollisionGrid();

    const NavigationGrid& getNavigationGrid() const { return navigationGrid; }
    void rebuildNavigationGrid();

    // Safe zone
    bool isInSafeZone(const Vector3& position, float margin = 0.0f) const;
    void setPlayerInsideSafeZone(bool inside) { playerInsideSafeZone = inside; }
    bool isPlayerInsideSafeZone() const { return playerInsideSafeZone; }

    // Lighting system
    void setLighting(Lighting* light) { lighting = light; }
    Lighting* getLighting() const { return lighting; }

    // Scene bounds calculation for camera zoom-to-fit
    void calculateSceneBounds(Vector3& center, float& radius) const;

    void switchTexture();

private:
    void drawGround() const;
    void drawBoundaryWalls() const;
    void drawSafeZoneIndicator() const;
    void drawGlassPanels() const;
    // void updateBullets(float deltaTime);
    void checkBulletCollisions();

    std::vector<GameObject*> gameObjects;
    std::vector<Bullet*> bullets;
    std::vector<Texture*> textures;
    std::vector<Target*> targets;
    std::vector<std::shared_ptr<Shape>> objects;
    std::vector<Enemy*> enemies;

    struct GlassPanel {
        Vector3 center;
        float width;
        float height;
        bool facingX;
        float normalSign;
    };
    std::vector<GlassPanel> glassPanels;
    void addGlassPanel(const Vector3& center, float width, float height, bool facingX, float normalSign);

    float groundSize;
    Color groundColor;
    float wallHeight;
    float wallThickness;

    Lighting* lighting;  // Not owned by Scene, just a reference

    // Grid-based collision detection
    CollisionGrid collisionGrid;
    NavigationGrid navigationGrid;

    static constexpr float SAFE_ZONE_RADIUS = 4.0f;
    static constexpr float SAFE_ZONE_LIGHT_HEIGHT = 15.0f;
    static constexpr int SAFE_ZONE_CIRCLE_SEGMENTS = 64;

    bool playerInsideSafeZone;
};
