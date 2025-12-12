#pragma once

#include "Shapes.h"

class Enemy
{
public:
    enum PartType {BODY, HEAD};
    Enemy(Vector3 ipos, Color icol = Color());
    Enemy(Vector3 ipos, Color icolHead, Color icolBody);
    void draw();

    void setPosition(Vector3 ipos) {position = ipos; updatePos();}
    void setColor(Color icolor, PartType part);
    void takeDamage(float damage);

    Vector3 getPosition() {return position;}
    Color getColor(PartType part) {return part == BODY ? body->getColor() : head->getColor();}
    bool isAlive() {return alive;}
    Shape* getBodyShape() {return (Shape*)body;}
    Shape* getHeadShape() {return (Shape*)head;}
private:
    Cylinder* body;
    Sphere* head;
    Vector3 position;

    float currentHealth;
    float maxHealth;
    bool alive;

    void updatePos();
    void drawHealthBar();
};
