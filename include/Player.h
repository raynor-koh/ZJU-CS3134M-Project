#pragma once
#include <vector>
#include <memory>
#include "Shapes.h"
#include "Enemy.h"
#include "Scene.h"

class Player
{
public:
    enum PartType {BODY, HEAD};
    Player(Scene* scene) : scene(scene) {init();}
    void draw();
    void drawCoordinateAxes();

    void setPosition(Vector3 ipos) {position = ipos; updatePos();}
    void setColor(Color icolor, PartType part);
    void setVisible(bool vis) {visible = vis;}
    void setScene(Scene* scene) {this->scene = scene;}
    void setTestDraw(bool val) {testDraw = val;}

    Vector3 getPosition() {return position;}
    Color getColor(PartType part) {return part == BODY ? body->getColor() : head->getColor();}
    Vector3 getVisionDirection() {return visionDirection;}

    void moveAbsolute(float dx, float dy, float dz);
    void updateVisionDirection(Vector3 dir) {visionDirection = dir.normalized();}
    bool checkSceneCollision();


    void update(float deltaTime);  // Update physics (gravity, jumping)
    void jump();  // Trigger jump

    // Health system
    void takeDamage(float damage);
    void heal(float amount);
    void resetHealth();
    float getHealth() const { return currentHealth; }
    float getMaxHealth() const { return maxHealth; }
    float getHealthPercent() const { return currentHealth / maxHealth; }
    bool isAlive() const { return currentHealth > 0.0f; }

    // Collision with enemies (made public for EnemyManager)
    bool checkCollision(const std::vector<Enemy*>& enemies);

private:
    Cylinder* body;
    Sphere* head;
    Scene* scene;

    Vector3 position;
    Vector3 visionDirection;
    bool visible;
    bool testDraw;

    void init();
    void updatePos();
    void drawDirection();
    void drawHealthBar();  // Draw player health bar on screen

    bool checkCollision(const std::vector<std::shared_ptr<Shape>>& objects);
    friend class Enemy;
    friend class EnemyManager;

    float verticalVelocity; // Vertical velocity for jumping
    float groundLevel;      // Y position of the ground
    bool isOnGround;        // Whether Stob is on the ground
    static constexpr float GRAVITY = 20.0f;      // Gravity acceleration
    static constexpr float JUMP_VELOCITY = 8.0f; // Initial jump velocity

    // Health system
    float currentHealth;
    float maxHealth;
    static constexpr float DEFAULT_MAX_HEALTH = 100.0f;
};