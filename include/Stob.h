#pragma once
#include <GL/glew.h>
#include <vector>
#include "Texture.h"
#include "GameObject.h"

class Stob : GameObject {
public:
    enum PartType {SIDE=0, CAP=1, TEXTURE=2};
    Stob(Vector3 pos, float ih, float idiameter, Color col = Color(1.0f, 1.0f, 1.0f));
    void draw();
    bool testCollision(Vector3 posBullet, float rBullet);
    void setSlices(int s) { slices = s;}
    void rebindTexture(enum PartType type, char* bmpFile);
    void setTextureMode(bool enabled) { textureEnabled = enabled; }
    void setColor(enum PartType type, Color icolor);
private:
    Vector3 position;
    Vector3 size;
    float diameter;
    int slices;
    Color colorSide, colorCap, color;
    std::vector<Texture> textures;
    GLubyte image[imageheight][imageweight][1];
    bool textureEnabled;
    void init();
};

