#pragma once
#include <GL/glew.h>
#include <vector>
#include "Texture.h"

class Stob {
public:
    float x, y, z, h, radius;
    enum TextureType {SIDE=0, CAP=1};
    // Input: postion(centerX,bottomY,centerZ), height, radius
    Stob(float ix, float iy, float iz, float ih, float iradius)
        : x(ix), y(iy), z(iz), h(ih), radius(iradius) {init();}
    Stob(float* posv, float ih, float iradius)
        : x(posv[0]), y(posv[1]), z(posv[2]), h(ih), radius(iradius) {init();}
    void draw();
    // Input: position and radius of bullet
    bool testCollision(float px, float py, float pz, float rBullet);
    void setSlices(int s) { slices = s;}
    void rebindTexture(enum TextureType type, char* bmpFile);
    void setTextureMode(bool enabled) { textureEnabled = enabled; }
private:
    int slices;
    std::vector<Texture> textures;
    GLubyte image[imageheight][imageweight][1];
    bool textureEnabled;
    void init();
};