#include "Bullet.h"

Bullet::Bullet(Vector3 ipos, Vector3 idirection, float damage) : position(ipos), direction(idirection.normalized()), damage(damage),
    life(0.0f), maxLife(5.0f), active(true), speed(16.0f) {
    sphere = new Sphere(ipos, 0.1f, Color(0.0f, 1.0f, 1.0f));
}

void Bullet::draw() {
    sphere->draw();
}

void Bullet::move(float dt) {
    position = position + direction * dt * speed;
    sphere->setPosition(position);
    life += dt;
}

void Bullet::setColor(Color icolor) {
    sphere->setColor(icolor);
}
