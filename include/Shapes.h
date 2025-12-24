#pragma once
#include "GameObject.h"
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <variant>
#include <vector>
#include "Texture.h"

class Shape {
public:
    enum ShapeType {CYLINDER, SPHERE, CUBE, GENERIC_MESH};
    Shape(Vector3 ipos = Vector3(), Vector3 isize = Vector3(), Color icol = Color());
    virtual void draw() = 0;

    // Tessellate shape into vertices, normals, and triangle indices (local space)
    virtual void tessellate(std::vector<Vector3>& positions,
                           std::vector<Vector3>& normals,
                           std::vector<int>& indices) const {
        // Default implementation - empty geometry
        positions.clear();
        normals.clear();
        indices.clear();
    }

    void setColor(Color icolor) {color = icolor;}
    void setPosition(Vector3 ipos) {pos = ipos;}
    void setSize(Vector3 isize) {size = isize;}
    void setAxis(Vector3 iaxis, float iaxisAngle) { axis = iaxis; axisAngle = iaxisAngle; }

    Color getColor() const {return color;}
    Vector3 getPosition() const {return pos;}
    Vector3 getSize() const {return size;}
    Vector3 getAxis() const {return axis;}
    float getAxisAngle() const {return axisAngle;}
    ShapeType getType() const {return type;}

    enum PartType {SIDE=0, CAP=1, BOTH=2};
    virtual void bindTexture(Texture* texture, PartType type = BOTH) = 0;
    void setTextureMode(bool enabled) { textureEnabled = enabled; }
protected:
    ShapeType type;
    Vector3 pos, size;
    Vector3 axis;
    float axisAngle;
    Color color;
    CollisionType collisionType;

    bool textureEnabled;
};

class Cylinder : public Shape {
public:
    // enum PartType {SIDE=0, CAP=1, BOTH=2};
    Cylinder(Vector3 ipos, float ih, float idiameter, Color icol = Color());
    Cylinder(Vector3 ipos, float ih, float idiameter, Color icolSide, Color icolCap);
    Cylinder(Vector3 ipos, float ih, float idiameter, Vector3 iaxis, float iaxisAngle);
    void draw();
    void tessellate(std::vector<Vector3>& positions,
                   std::vector<Vector3>& normals,
                   std::vector<int>& indices) const override;

    void setSlices(int s) { slices = s;}
    void bindTexture(Texture* texture, enum PartType type);
    void setColor(Color icolor, enum PartType type = BOTH);
    // void setTextureMode(bool enabled) { textureEnabled = enabled; }

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
    // bool textureEnabled;
    void init();
};

class Sphere : public Shape {
public:
    Sphere(Vector3 ipos, float idiameter, Color icol = Color());
    Sphere(Vector3 ipos, float idiameter, Vector3 iRotationAxis, float iRotationAngle, Color icol);
    void draw();
    void tessellate(std::vector<Vector3>& positions,
                   std::vector<Vector3>& normals,
                   std::vector<int>& indices) const override;

    void setSlices(int slice, int snack) { slices = slice; stacks = snack; }
    void bindTexture(Texture* itexture, PartType type) { texture = itexture; setTextureMode(true); }
    void setColor(Color icolor) {color = icolor;}
    // void setTextureMode(bool enabled) { textureEnabled = enabled; }
    void setRotation(Vector3 iRotationAxis, float iRotationAngle) { axis = iRotationAxis; axisAngle = iRotationAngle; }

    Vector3 getPosition() {return pos;}
    float getRadius() {return radius;}
private:
    Vector3 axis;
    float radius, axisAngle;
    int slices, stacks;
    Texture *texture;
    // bool textureEnabled;
    void init();
};

class Cube : public Shape {
public:
    Cube(Vector3 ipos, Vector3 isize, Color icol = Color());
    void draw();
    void tessellate(std::vector<Vector3>& positions,
                   std::vector<Vector3>& normals,
                   std::vector<int>& indices) const override;

    void setColor(Color icolor) {color = icolor;}
    void bindTexture(Texture* itexture, PartType type) {texture = itexture; setTextureMode(true);}
    // void setTextureMode(bool enabled) { textureEnabled = enabled;}

    Vector3 getPosition() {return pos;}
    Vector3 getSize() {return size;}
private:
    Texture *texture;
    // bool textureEnabled;
    void init();
};

class Cone : public Shape {
public:
    Cone(Vector3 ipos, float height, float baseDiameter, Color icol = Color());
    void draw();
    void tessellate(std::vector<Vector3>& positions,
                   std::vector<Vector3>& normals,
                   std::vector<int>& indices) const override;

    void setSlices(int s) { slices = s; }
    void setColor(Color icolor) { color = icolor; }

    Vector3 getPosition() { return pos; }
    Vector3 getSize() { return size; }

    void bindTexture(Texture* texture, PartType type) {}
private:
    float height, baseRadius;
    int slices;
    void init();
};

class Prism : public Shape {
public:
    Prism(Vector3 ipos, float height, float diameter, int sides = 6, Color icol = Color());
    void draw();
    void tessellate(std::vector<Vector3>& positions,
                   std::vector<Vector3>& normals,
                   std::vector<int>& indices) const override;

    void setSides(int s) { if (s >= 3) sides = s; }
    void setColor(Color icolor) { color = icolor; }

    Vector3 getPosition() { return pos; }
    Vector3 getSize() { return size; }

    void bindTexture(Texture* texture, PartType type) {}
private:
    float height, radius;
    int sides;
    void init();
};

class Frustum : public Shape {
public:
    Frustum(Vector3 ipos, float height, float bottomDiameter, float topDiameter, int sides = 4, Color icol = Color());
    void draw();
    void tessellate(std::vector<Vector3>& positions,
                   std::vector<Vector3>& normals,
                   std::vector<int>& indices) const override;

    void setSides(int s) { if (s >= 3) sides = s; }
    void setColor(Color icolor) { color = icolor; }

    Vector3 getPosition() { return pos; }
    Vector3 getSize() { return size; }
    
    void bindTexture(Texture* texture, PartType type) {}
private:
    float height, bottomRadius, topRadius;
    int sides;
    void init();
};
