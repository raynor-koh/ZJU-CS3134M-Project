#pragma once
#include <GL/glew.h>
#include <vector>
#include "Texture.h"
#include "GameObject.h"

class Stob {
public:
    float x, y, z, h, radius;
    enum PartType {SIDE=0, CAP=1};
    // Input: postion(centerX,bottomY,centerZ), height, radius
    Stob(Vector3 pos, float ih, float iradius)
        : x(pos.x), y(pos.y), z(pos.z), h(ih), radius(iradius) {init();}
    void draw();
    // Input: position and radius of bullet
    bool testCollision(Vector3 posBullet, float rBullet);
    void setSlices(int s) { slices = s;}
    void rebindTexture(enum PartType type, char* bmpFile);
    void setTextureMode(bool enabled) { textureEnabled = enabled; }
    void setColor(enum PartType type, Color color);
private:
    int slices;
    Color colorSide, colorCap;
    std::vector<Texture> textures;
    GLubyte image[imageheight][imageweight][1];
    bool textureEnabled;
    void init();
};
