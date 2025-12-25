#define _USE_MATH_DEFINES
#include "Stob.h"
#include "Scene.h"
#include <cmath>
#include <gl/glut.h>
#include <iostream>
#define disp(x) std::cout << x << std::endl;

Stob::Stob(Vector3 pos, float ih, float idiameter, Color col)
    : GameObject(pos, Vector3(idiameter, ih, idiameter), col), position(pos), size(Vector3(idiameter, ih, idiameter)),
      diameter(idiameter), scene(nullptr), verticalVelocity(0.0f), groundLevel(0.0f), isOnGround(true) {
    init();
}

bool Stob::testCollision(Vector3 posBullet, float rBullet) {
    float dx = posBullet.x - position.x;
    float dz = posBullet.z - position.z;
    float distXZ = sqrtf(dx * dx + dz * dz);
    if(distXZ <= (diameter/2.0f + rBullet) && posBullet.y >= position.y && posBullet.y <= (position.y + size.y)) {
        return true;
    }
    return false;
}

void Stob::rebindTexture(enum PartType type, char* bmpFile) {
    if(type < 2) textures[type].rebind(bmpFile);
}

void Stob::setColor(enum PartType type, Color icolor) {
    if(type == CAP) {
        colorCap = icolor;
    } else if(type == SIDE) {
        colorSide = icolor;
    } else if(type == TEXTURE) {
        color = icolor;
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
void Stob::move_absolute(float dx, float dy, float dz) {
    // Calculate new position
    float newX = position.x + dx;
    float newZ = position.z + dz;

    // Check collision before moving
    // Note: diameter is actually the radius of the rendered cylinder
    if (scene == nullptr || !scene->checkCollision(newX, newZ, diameter)) {
        position.x = newX;
        position.y += dy;
        position.z = newZ;
    }
}

void Stob::drawDirection() {//绘制主角头顶的方向箭头，指向鼠标位置
    glPushMatrix();

    // 平移到 Stob 的位置
    glTranslatef(position.x, position.y + size.y , position.z); // 箭头位于 Stob 的顶部中心

    // 根据朝向旋转
    float angle = atan2(visionDirection.z, visionDirection.x) * 180.0f / M_PI; // 计算朝向角度
    glRotatef(-angle - 90.0f, 0.0f, 1.0f, 0.0f); // 绕 Y 轴旋转

    // 设置箭头颜色
    glColor3f(1.0f, 1.0f, 0.0f);

    // 绘制箭头
    glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.1f, 0.4f);   // 箭头前端
        glVertex3f(-0.2f, 0.1f, 0.0f);  // 箭头左后
        glVertex3f(0.2f, 0.1f, 0.0f);   // 箭头右后
    glEnd();
    
    glPopMatrix();
}

void Stob::drawCoordinateAxes(float x, float y, float z) {
    glDisable(GL_LIGHTING);  // 临时禁用光照
    
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    
    // X轴 - 红色 (x方向)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x, y, z);
    glVertex3f(x + 2.0f, y, z);
    
    // Y轴 - 绿色 (y方向)  
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x, y, z);
    glVertex3f(x, y + 2.0f, z);
    
    // Z轴 - 蓝色 
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x, y, z);
    glVertex3f(x, y, z + 2.0f);
    
    glEnd();
    
    
    glEnable(GL_LIGHTING);
}

void Stob::draw() {
    if (!visible) return; // Don't draw if not visible

    glPushMatrix();
    glTranslatef(position.x, position.y, position.z);
    glScalef(size.x, size.y, size.z);
    for(int i=0; i<slices; i++) {
        float theta1 = (2.0f * M_PI * i) / slices;
        float theta2 = (2.0f * M_PI * (i + 1)) / slices;

        if(textureEnabled) glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textures[1].getID());
        glBegin(GL_TRIANGLES);
        if(textureEnabled) glColor3f(color.r, color.g, color.b);
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

        glBindTexture(GL_TEXTURE_2D, textures[0].getID());
        glBegin(GL_QUADS);
        if(textureEnabled) glColor3f(color.r, color.g, color.b);
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
    if(testdraw) {
        // 注意：需要先恢复坐标系
        glPopMatrix(); // 结束圆柱的变换
        
        // 然后绘制箭头和坐标轴
        drawDirection();
        drawCoordinateAxes(position.x, position.y, position.z);
        
        // 重新推入矩阵，以便后续绘制（如果有）
        glPushMatrix();
    }
    glPopMatrix();
}

void Stob::update(float deltaTime) {
    // Apply gravity
    if (!isOnGround) {
        verticalVelocity -= GRAVITY * deltaTime;
    }

    // Update Y position based on vertical velocity
    position.y += verticalVelocity * deltaTime;

    // Ground collision
    if (position.y <= groundLevel) {
        position.y = groundLevel;
        verticalVelocity = 0.0f;
        isOnGround = true;
    } else {
        isOnGround = false;
    }
}

void Stob::jump() {
    if (isOnGround) {
        verticalVelocity = JUMP_VELOCITY;
        isOnGround = false;
    }
}
