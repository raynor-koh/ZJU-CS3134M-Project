#pragma once
#include "GameObject.h"
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <variant>
#include "Texture.h"


class Shape {
public:
    enum ShapeType {CYLINDER, SPHERE, CUBE};
    Shape(Vector3 ipos = Vector3(), Vector3 isize = Vector3(), Color icol = Color());
    virtual void draw() = 0;

    void setColor(Color icolor) {color = icolor;}
    void setPosition(Vector3 ipos) {pos = ipos;}
    void setSize(Vector3 isize) {size = isize;}
    void setAxis(Vector3 iaxis, float iaxisAngle) { axis = iaxis; axisAngle = iaxisAngle; }

    Color getColor() {return color;}
    Vector3 getPosition() {return pos;}
    Vector3 getSize() {return size;}
    Vector3 getAxis() {return axis;}
    Vector3 getAxisAngle() {return axisAngle;}
    ShapeType getType() {return type;}
protected:
    ShapeType type;
    Vector3 pos, size;
    Vector3 axis;
    float axisAngle;
    Color color;
    CollisionType collisionType;
};

class Cylinder : public Shape {
public:
    enum PartType {SIDE=0, CAP=1, BOTH=2};
    Cylinder(Vector3 ipos, float ih, float idiameter, Color icol = Color());
    Cylinder(Vector3 ipos, float ih, float idiameter, Color icolSide, Color icolCap);
    Cylinder(Vector3 ipos, float ih, float idiameter, Vector3 iaxis, float iaxisAngle);
    void draw();

    void setSlices(int s) { slices = s;}
    void bindTexture(Texture* texture, enum PartType type);
    void setColor(Color icolor, enum PartType type = BOTH);
    void setTextureMode(bool enabled) { textureEnabled = enabled; }

    Vector3 getPosition() {return pos;}
    Vector3 getSize() {return size;}
    Vector3 getTopCenter() {return pos + axis * (height / 2.0f);}
    Vector3 getBottomCenter() {return pos - axis * (height / 2.0f);}
    float getBoundingSphereRadius() {return std::sqrt(radius * radius + height * height / 4.0f);}
private:
    Vector3 axis;
    float height, radius, axisAngle;
    int slices;
    Color colorSide, colorCap, color;
    Texture *textureSide, *textureCap;
    bool textureEnabled;
    void init();
};

class Sphere : public Shape {
public:
    Sphere(Vector3 ipos, float idiameter, Color icol = Color());
    Sphere(Vector3 ipos, float idiameter, Vector3 iRotationAxis, float iRotationAngle, Color icol);
    void draw();

    void setSlices(int slice, int snack) { slices = slice; stacks = snack; }
    void bindTexture(Texture* itexture) { texture = itexture; }
    void setColor(Color icolor) {color = icolor;}
    void setTextureMode(bool enabled) { textureEnabled = enabled; }
    void setRotation(Vector3 iRotationAxis, float iRotationAngle) { axis = iRotationAxis; axisAngle = iRotationAngle; }

    Vector3 getPosition() {return pos;}
    float getRadius() {return radius;}
private:
    Vector3 axis;
    float radius, axisAngle;
    int slices, stacks;
    Texture *texture;
    bool textureEnabled;
    void init();
};

class Cube : public Shape {
public:
    Cube(Vector3 ipos, Vector3 isize, Color icol = Color());
    void draw();

    void setColor(Color icolor) {color = icolor;}
    void bindTexture(Texture* itexture) {texture = itexture;}
    void setTextureMode(bool enabled) { textureEnabled = enabled;}

    Vector3 getPosition() {return pos;}
    Vector3 getSize() {return size;}
private:
    Texture *texture;
    bool textureEnabled;
    void init();
};
