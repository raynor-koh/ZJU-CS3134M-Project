#pragma once
#include "Shapes.h"

class Bullet
{
public:
    Bullet(Vector3 ipos, Vector3 idirection, float idamage = 1.0f);
    void draw();
    void move(float dt);

    void setPosition(Vector3 ipos) {position = ipos;}
    void setColor(Color icolor);
    void setDamage(float d) {damage = d;}
    void deactivate() {active = false;}

    Vector3 getPosition() {return position;}
    Color getColor() {return sphere->getColor();}
    Shape* getShape() {return (Shape*)sphere;}
    float getDamage() {return damage;}
    bool isActive() {return active;}
    
    bool shouldRemove() const {return !active || life >= maxLife ;};
private:
    Sphere* sphere;
    Vector3 position;
    Vector3 direction;
    float speed;

    bool active;

    float damage;
    float life;
    float maxLife;
};
