#define _USE_MATH_DEFINES
#include "Stob.h"
#include <cmath>
#include <gl/glut.h>
#include <iostream>
#define disp(x) std::cout << x << std::endl;

bool Stob::testCollision(Vector3 posBullet, float rBullet) {
    float dx = posBullet.x - x;
    float dz = posBullet.z - z;
    float distXZ = sqrtf(dx * dx + dz * dz);
    if(distXZ <= (radius + rBullet) && posBullet.y >= y && posBullet.y <= (y + h)) {
        return true;
    }
    return false;
}

void Stob::rebindTexture(enum PartType type, char* bmpFile) {
    textures[type].rebind(bmpFile);
}

void Stob::setColor(enum PartType type, Color color) {
    if(type == CAP) {
        colorCap = color;
    } else if(type == SIDE) {
        colorSide = color;
    }
}

void Stob::init() {
    slices = 20;
    textureEnabled = true;
    colorSide = Color(0.2f, 0.8f, 0.2f);
    colorCap = Color(0.8f, 0.2f, 0.8f);

    Texture textureside("resources/textures/WoodSide.bmp");
    textures.push_back(textureside);
    Texture texturecap("resources/textures/WoodCap.bmp");
    textures.push_back(texturecap);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, 5, imageweight, imageheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Stob::draw() {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(radius, h, radius);
    for(int i=0; i<slices; i++) {
        float theta1 = (2.0f * M_PI * i) / slices;
        float theta2 = (2.0f * M_PI * (i + 1)) / slices;

        if(textureEnabled) glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[1].ID);
        glBegin(GL_TRIANGLES);
        if(textureEnabled) glColor3f(1.0f, 1.0f, 1.0f);
        else glColor3f(colorCap.r, colorCap.g, colorCap.b);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta2), 0.5f + 0.5f * sinf(theta2));
        glVertex3f(cosf(theta2), 0.0f, sinf(theta2));
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta1), 0.5f + 0.5f * sinf(theta1));
        glVertex3f(cosf(theta1), 0.0f, sinf(theta1));

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f, 0.5f);
        glVertex3f(0.0f, 1.0f, 0.0f);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta1), 0.5f + 0.5f * sinf(theta1));
        glVertex3f(cosf(theta1), 1.0f, sinf(theta1));
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.5f + 0.5f * cosf(theta2), 0.5f + 0.5f * sinf(theta2));
        glVertex3f(cosf(theta2), 1.0f, sinf(theta2));
        glEnd();

        glBindTexture(GL_TEXTURE_2D, textures[0].ID);
        glBegin(GL_QUADS);
        if(textureEnabled) glColor3f(1.0f, 1.0f, 1.0f);
        else glColor3f(colorSide.r, colorSide.g, colorSide.b);
        glNormal3f(cosf(theta1), 0.0f, sinf(theta1));
        glTexCoord2f((float)i / slices, 0.0f);
        glVertex3f(cosf(theta1), 0.0f, sinf(theta1));
        glNormal3f(cosf(theta2), 0.0f, sinf(theta2));
        glTexCoord2f((float)(i + 1) / slices, 0.0f);
        glVertex3f(cosf(theta2), 0.0f, sinf(theta2));
        glNormal3f(cosf(theta2), 0.0f, sinf(theta2));
        glTexCoord2f((float)(i + 1) / slices, 1.0f);
        glVertex3f(cosf(theta2), 1.0f, sinf(theta2));
        glNormal3f(cosf(theta1), 0.0f, sinf(theta1));
        glTexCoord2f((float)i / slices, 1.0f);
        glVertex3f(cosf(theta1), 1.0f, sinf(theta1));
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}
