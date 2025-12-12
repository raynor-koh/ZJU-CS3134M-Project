#define _USE_MATH_DEFINES
#include "Shapes.h"
#include "CollisionDetector.h"
#include <gl/glut.h>
#include <iostream>
#include <cmath>

Shape::Shape(Vector3 ipos, Vector3 isize, Color icol)
    : pos(ipos), size(isize), color(icol), axis(Vector3(0.0f, 1.0f, 0.0f)), axisAngle(0.0f) {}

Cylinder::Cylinder(Vector3 ipos, float ih, float idiameter, Color icol)
    : Shape(ipos, Vector3(idiameter, ih, idiameter), icol), 
      height(ih), radius(idiameter/2), colorSide(icol), colorCap(icol) {
    init();
    axis = Vector3(0.0f, 1.0f, 0.0f);
    axisAngle = 0.0f;
}

Cylinder::Cylinder(Vector3 ipos, float ih, float idiameter, Color icolSide, Color icolCap) 
    : Shape(ipos, Vector3(idiameter, ih, idiameter), Color()), 
      height(ih), radius(idiameter/2), colorSide(icolSide), colorCap(icolCap) {
    init();
    axis = Vector3(0.0f, 1.0f, 0.0f);
    axisAngle = 0.0f;
}

Cylinder::Cylinder(Vector3 ipos, float ih, float idiameter, Vector3 iaxis, float iaxisAngle)
    : Shape(ipos, Vector3(idiameter, ih, idiameter), Color(1.0f, 1.0f, 1.0f)), 
      height(ih), radius(idiameter/2), axis(iaxis.normalized()), axisAngle(iaxisAngle) , colorSide(Color()), colorCap(Color()) {
    init();
}

void Cylinder::init() {
    type = CYLINDER;
    slices = 20;
    textureEnabled = false;
}

void Cylinder::draw() {
    bool textureDraw = textureEnabled && (textureSide != NULL && textureCap != NULL);
    if(textureDraw) glEnable(GL_TEXTURE_2D);
    Vector3 tempAxis = axis.cross(Vector3(0.0f, 1.0f, 0.0f));
    float tempAngle = acos(axis.dot(Vector3(0.0f, 1.0f, 0.0f))) * 180.0f / M_PI;
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    if(tempAxis.length() > 1e-9) glRotatef(tempAngle, tempAxis.x, tempAxis.y, tempAxis.z);
    glScalef(size.x/2, size.y, size.z/2);
    glRotatef(axisAngle, 0.0f, 1.0f, 0.0f);
    for(int i=0; i<slices; i++) {
        float theta1 = (2.0f * M_PI * i) / slices;
        float theta2 = (2.0f * M_PI * (i + 1)) / slices;
        if(textureDraw) glBindTexture(GL_TEXTURE_2D, textureSide->ID);
        glBegin(GL_TRIANGLES);
        if(textureDraw) glColor3f(color.r, color.g, color.b);
        else glColor3f(colorCap.r, colorCap.g, colorCap.b);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, -0.5f, 0.0f);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta2), 0.5f + 0.5f * sinf(theta2));
        glVertex3f(cosf(theta2), -0.5f, sinf(theta2));
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta1), 0.5f + 0.5f * sinf(theta1));
        glVertex3f(cosf(theta1), -0.5f, sinf(theta1));

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, 0.5f, 0.0f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta1), 0.5f + 0.5f * sinf(theta1));
        glVertex3f(cosf(theta1), 0.5f, sinf(theta1));
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta2), 0.5f + 0.5f * sinf(theta2));
        glVertex3f(cosf(theta2), 0.5f, sinf(theta2));
        glEnd();

        if(textureDraw) glBindTexture(GL_TEXTURE_2D, textureSide->ID);
        glBegin(GL_QUADS);
        if(textureDraw) glColor3f(color.r, color.g, color.b);
        else glColor3f(colorSide.r, colorSide.g, colorSide.b);
        glNormal3f(cosf(theta1), 0.0f, sinf(theta1));
        glTexCoord2f((float)i / slices, 0.0f);
        glVertex3f(cosf(theta1), -0.5f, sinf(theta1));
        glNormal3f(cosf(theta2), 0.0f, sinf(theta2));
        glTexCoord2f((float)(i + 1) / slices, 0.0f);
        glVertex3f(cosf(theta2), -0.5f, sinf(theta2));
        glNormal3f(cosf(theta2), 0.0f, sinf(theta2));
        glTexCoord2f((float)(i + 1) / slices, 1.0f);
        glVertex3f(cosf(theta2), 0.5f, sinf(theta2));
        glNormal3f(cosf(theta1), 0.0f, sinf(theta1));
        glTexCoord2f((float)i / slices, 1.0f);
        glVertex3f(cosf(theta1), 0.5f, sinf(theta1));
        glEnd();
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void Cylinder::bindTexture(Texture* texture, enum PartType type) {
    if(type == SIDE) textureSide = texture;
    else if(type == CAP) textureCap = texture;
}

void Cylinder::setColor(Color icolor, enum PartType type) {
    if(type == SIDE) colorSide = icolor;
    else if(type == CAP) colorCap = icolor;
    else if(type == BOTH) colorSide = colorCap = icolor;
}

Sphere::Sphere(Vector3 ipos, float idiameter, Color icol)
    : Shape(ipos, Vector3(idiameter, idiameter, idiameter), icol), 
      radius(idiameter/2.0f) {
        init();
        axis = Vector3(0.0f, 1.0f, 0.0f);
        axisAngle = 0.0f;
    }

Sphere::Sphere(Vector3 ipos, float idiameter, Vector3 iRotationAxis, float iRotationAngle, Color icol)
    : Shape(ipos, Vector3(idiameter, idiameter, idiameter), icol), 
      radius(idiameter / 2.0f), axis(iRotationAxis), axisAngle(iRotationAngle) {
        init();
    }

void Sphere::init() {
    type = SPHERE;
    slices = 20;
    stacks = 20;
    textureEnabled = false;
}

void Sphere::draw() {
    bool textureDraw = textureEnabled && (texture != NULL);
    if(textureDraw) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture->ID);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(color.r, color.g, color.b);
    }
    Vector3 tempAxis = axis.cross(Vector3(0.0f, 1.0f, 0.0f));
    float tempAngle = acos(axis.dot(Vector3(0.0f, 1.0f, 0.0f))) * 180.0f / M_PI;
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    if(tempAxis.length() > 1e-9) glRotatef(axisAngle, tempAxis.x, tempAxis.y, tempAxis.z);
    glScalef(radius, radius, radius);
    glRotatef(axisAngle, 0.0f, 1.0f, 0.0f);
    for(int i=0; i<slices; i++) {
        float theta1 = (2.0f * M_PI * i) / slices;
        float theta2 = (2.0f * M_PI * (i + 1)) / slices;
        for(int j=0; j<stacks; j++) { 
            float phi1 = (M_PI * j) / stacks;
            float phi2 = (M_PI * (j + 1)) / stacks;
            glBegin(GL_QUADS);
            glNormal3f(cosf(theta1) * sinf(phi1), sinf(theta1) * sinf(phi1), cosf(phi1));
            glTexCoord2f(1.0f * i / slices, 1.0f * j / stacks);
            glVertex3f(cosf(theta1) * sinf(phi1), sinf(theta1) * sinf(phi1), cosf(phi1));
            glNormal3f(cosf(theta2) * sinf(phi1), sinf(theta2) * sinf(phi1), cosf(phi1));
            glTexCoord2f(1.0f * (i + 1) / slices, 1.0f * j / stacks);
            glVertex3f(cosf(theta2) * sinf(phi1), sinf(theta2) * sinf(phi1), cosf(phi1));
            glNormal3f(cosf(theta2) * sinf(phi2), sinf(theta2) * sinf(phi2), cosf(phi2));
            glTexCoord2f(1.0f * (i + 1) / slices, 1.0f * (j + 1) / stacks);
            glVertex3f(cosf(theta2) * sinf(phi2), sinf(theta2) * sinf(phi2), cosf(phi2));
            glNormal3f(cosf(theta1) * sinf(phi2), sinf(theta1) * sinf(phi2), cosf(phi2));
            glTexCoord2f(1.0f * i / slices, 1.0f * (j + 1) / stacks);
            glVertex3f(cosf(theta1) * sinf(phi2), sinf(theta1) * sinf(phi2), cosf(phi2));
            glEnd();
        }
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

Cube::Cube(Vector3 ipos, Vector3 isize, Color icol)
    : Shape(ipos, isize/2.0f, icol) {
    init();
}

void Cube::init() {
    type = CUBE;
    textureEnabled = false;
}

void Cube::draw() {
    bool textureDraw = textureEnabled && (texture != NULL);
    if(textureDraw) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture->ID);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(color.r, color.g, color.b);
    }
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glScalef(size.x, size.y, size.z);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glEnd();
    glPopMatrix();
}