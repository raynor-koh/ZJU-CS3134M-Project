#include "GameObject.h"
#include <GL/glut.h>

GameObject::GameObject(const Vector3& position, const Vector3& size, const Color& color)
    : position(position), size(size), color(color) {
}

void GameObject::draw() const {
    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glColor3f(color.r, color.g, color.b);
    glScalef(size.x, size.y, size.z);
    glutSolidCube(1.0f);
    glPopMatrix();
}
