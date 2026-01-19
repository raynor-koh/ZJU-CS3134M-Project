#pragma once

#include "Shapes.h"

class Enemy
{
public:
    enum PartType {BODY, HEAD};
    Enemy(Vector3 ipos, Color icol = Color());
    Enemy(Vector3 ipos, Color icolHead, Color icolBody);
    ~Enemy();  // Destructor to clean up shape pointers
    void draw();

    void setPosition(Vector3 ipos) {position = ipos; updatePos();}
    void setColor(Color icolor, PartType part);
    void takeDamage(float damage);
    void setYaw(float angle) {yaw = angle; updatePos();}  // Set rotation angle (radians)

    Vector3 getPosition() {return position;}
    Color getColor(PartType part) {return part == BODY ? bodyLower->getColor() : head->getColor();}
    bool isAlive() {return alive;}
    float getYaw() {return yaw;}
    Shape* getBodyShape() {return (Shape*)bodyLower;}
    Shape* getHeadShape() {return (Shape*)head;}
private:
    // Body structure (2 spheres)
    Sphere* bodyLower;    // Large bottom sphere
    Sphere* head;         // Smaller top sphere

    // Decorations
    Cylinder* leftArm;    // Left stick arm
    Cylinder* rightArm;   // Right stick arm
    Cylinder* nose;       // Carrot nose (using Cylinder since Cone doesn't support rotation)
    Sphere* button1;      // Top coal button
    Sphere* button2;      // Middle coal button
    Sphere* button3;      // Bottom coal button
    Cylinder* hatBrim;    // Hat brim (flat disk)
    Cylinder* hatTop;     // Hat top (tall cylinder)
    Sphere* leftEye;      // Left eye
    Sphere* rightEye;     // Right eye

    Vector3 position;
    float yaw;  // Rotation angle around Y-axis (radians), 0 = facing +Z direction

    float currentHealth;
    float maxHealth;
    bool alive;

    void updatePos();
    void drawHealthBar();
};
