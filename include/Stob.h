#pragma once
#include <GL/glew.h>
#include <vector>
#include "Texture.h"
#include "GameObject.h"

class Scene; // Forward declaration

class Stob : GameObject {
public:
    enum PartType {SIDE=0, CAP=1, TEXTURE=2};
    Stob(Vector3 pos, float ih, float idiameter, Color col = Color(1.0f, 1.0f, 1.0f));
    void draw();
    bool testCollision(Vector3 posBullet, float rBullet);
    void setSlices(int s) { slices = s;}
    void rebindTexture(enum PartType type, char* bmpFile);
    void setTextureMode(bool enabled) { textureEnabled = enabled; }
    void setColor(enum PartType type, Color icolor);
    void move_absolute(float dx, float dy, float dz);
    void updateVisionDirection(Vector3 dir) { visionDirection = dir; }
    void drawDirection();
    void setTestDraw(bool val) { testdraw = val; }
    void drawCoordinateAxes(float x, float y, float z);
    void update(float deltaTime);  // Update physics (gravity, jumping)
    void jump();  // Trigger jump

    // Collision
    void setScene(Scene* scene) { this->scene = scene; }
    float getCollisionRadius() const { return diameter; } // diameter is actually the rendered radius

    // Shooting support
    Vector3 getPosition() const { return position; }
    void setPosition(const Vector3& pos) { position = pos; }
    Vector3 getVisionDirection() const { return visionDirection; }

    // Visibility control
    void setVisible(bool vis) { visible = vis; }
    bool isVisible() const { return visible; }

private:
    bool testdraw = false;
    bool visible = true;
    Vector3 position;//空间位置
    Vector3 visionDirection;//朝向
    Vector3 size;
    float diameter;
    int slices;
    Color colorSide, colorCap, color;
    std::vector<Texture> textures;
    GLubyte image[imageheight][imageweight][1];
    bool textureEnabled;
    Scene* scene;  // Scene reference for collision detection

    // Jump physics
    float verticalVelocity; // Vertical velocity for jumping
    float groundLevel;      // Y position of the ground
    bool isOnGround;        // Whether Stob is on the ground
    static constexpr float GRAVITY = 20.0f;      // Gravity acceleration
    static constexpr float JUMP_VELOCITY = 8.0f; // Initial jump velocity

    void init();
};

