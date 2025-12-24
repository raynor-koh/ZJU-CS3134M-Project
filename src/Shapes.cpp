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
    setTextureMode(texture != NULL);
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

// Tessellation implementations for mesh export

void Cylinder::tessellate(std::vector<Vector3>& positions,
                          std::vector<Vector3>& normals,
                          std::vector<int>& indices) const {
    positions.clear();
    normals.clear();
    indices.clear();

    // Generate vertices for bottom cap, top cap, and sides
    // Bottom cap center
    positions.push_back(Vector3(0.0f, -0.5f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    int bottomCenterIdx = 0;

    // Top cap center
    positions.push_back(Vector3(0.0f, 0.5f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    int topCenterIdx = 1;

    // Generate ring vertices for both caps and sides
    for (int i = 0; i <= slices; i++) {
        float theta = (2.0f * M_PI * i) / slices;
        float x = cosf(theta);
        float z = sinf(theta);

        // Bottom ring vertex (for caps)
        positions.push_back(Vector3(x, -0.5f, z));
        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));

        // Top ring vertex (for caps)
        positions.push_back(Vector3(x, 0.5f, z));
        normals.push_back(Vector3(0.0f, 1.0f, 0.0f));

        // Bottom ring vertex (for sides) - outward normals
        positions.push_back(Vector3(x, -0.5f, z));
        normals.push_back(Vector3(x, 0.0f, z).normalized());

        // Top ring vertex (for sides) - outward normals
        positions.push_back(Vector3(x, 0.5f, z));
        normals.push_back(Vector3(x, 0.0f, z).normalized());
    }

    // Generate indices for caps and sides
    for (int i = 0; i < slices; i++) {
        int baseIdx = 2 + i * 4;

        // Bottom cap triangle (center, edge1, edge2)
        indices.push_back(bottomCenterIdx);
        indices.push_back(baseIdx + 4);  // next bottom cap vertex
        indices.push_back(baseIdx);      // current bottom cap vertex

        // Top cap triangle (center, edge1, edge2)
        indices.push_back(topCenterIdx);
        indices.push_back(baseIdx + 1);  // current top cap vertex
        indices.push_back(baseIdx + 5);  // next top cap vertex

        // Side quad split into 2 triangles
        int sideCurBottom = baseIdx + 2;
        int sideCurTop = baseIdx + 3;
        int sideNextBottom = baseIdx + 6;
        int sideNextTop = baseIdx + 7;

        // Triangle 1
        indices.push_back(sideCurBottom);
        indices.push_back(sideNextBottom);
        indices.push_back(sideNextTop);

        // Triangle 2
        indices.push_back(sideCurBottom);
        indices.push_back(sideNextTop);
        indices.push_back(sideCurTop);
    }
}

void Sphere::tessellate(std::vector<Vector3>& positions,
                       std::vector<Vector3>& normals,
                       std::vector<int>& indices) const {
    positions.clear();
    normals.clear();
    indices.clear();

    // Generate vertices using spherical coordinates
    for (int j = 0; j <= stacks; j++) {
        float phi = (M_PI * j) / stacks;
        float sinPhi = sinf(phi);
        float cosPhi = cosf(phi);

        for (int i = 0; i <= slices; i++) {
            float theta = (2.0f * M_PI * i) / slices;
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            // Position (unit sphere, will be scaled later)
            float x = cosTheta * sinPhi;
            float y = sinTheta * sinPhi;
            float z = cosPhi;

            positions.push_back(Vector3(x, y, z));
            // For sphere, normal equals normalized position
            normals.push_back(Vector3(x, y, z).normalized());
        }
    }

    // Generate triangle indices from the grid
    for (int j = 0; j < stacks; j++) {
        for (int i = 0; i < slices; i++) {
            int idx0 = j * (slices + 1) + i;
            int idx1 = idx0 + 1;
            int idx2 = (j + 1) * (slices + 1) + i;
            int idx3 = idx2 + 1;

            // Quad split into 2 triangles
            // Triangle 1
            indices.push_back(idx0);
            indices.push_back(idx2);
            indices.push_back(idx1);

            // Triangle 2
            indices.push_back(idx1);
            indices.push_back(idx2);
            indices.push_back(idx3);
        }
    }
}

void Cube::tessellate(std::vector<Vector3>& positions,
                     std::vector<Vector3>& normals,
                     std::vector<int>& indices) const {
    positions.clear();
    normals.clear();
    indices.clear();

    // Cube vertices (8 corners, but we need 24 for unique normals per face)
    // Front face (+Z)
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));

    // Back face (-Z)
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    // Top face (+Y)
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));

    // Bottom face (-Y)
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));

    // Right face (+X)
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));

    // Left face (-X)
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));

    // Generate triangle indices (2 triangles per face = 12 triangles total)
    for (int face = 0; face < 6; face++) {
        int baseIdx = face * 4;
        // Triangle 1
        indices.push_back(baseIdx + 0);
        indices.push_back(baseIdx + 1);
        indices.push_back(baseIdx + 2);
        // Triangle 2
        indices.push_back(baseIdx + 0);
        indices.push_back(baseIdx + 2);
        indices.push_back(baseIdx + 3);
    }
}

// ========== CONE IMPLEMENTATION ==========

Cone::Cone(Vector3 ipos, float h, float baseDiameter, Color icol)
    : Shape(ipos, Vector3(baseDiameter, h, baseDiameter), icol),
      height(h), baseRadius(baseDiameter / 2.0f) {
    init();
}

void Cone::init() {
    type = CYLINDER; // Use CYLINDER type for collision (close enough)
    slices = 20;
}

void Cone::draw() {
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glColor3f(color.r, color.g, color.b);

    // Center the cone: base at -height/2, apex at +height/2
    float baseY = -height / 2.0f;
    float apexY = height / 2.0f;

    // Draw base cap (circle at bottom)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, baseY, 0.0f); // Center of base
    for (int i = 0; i <= slices; i++) {
        float theta = (2.0f * M_PI * i) / slices;
        float x = baseRadius * cosf(theta);
        float z = baseRadius * sinf(theta);
        glVertex3f(x, baseY, z);
    }
    glEnd();

    // Draw cone sides (triangles from base to apex)
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < slices; i++) {
        float theta1 = (2.0f * M_PI * i) / slices;
        float theta2 = (2.0f * M_PI * (i + 1)) / slices;

        float x1 = baseRadius * cosf(theta1);
        float z1 = baseRadius * sinf(theta1);
        float x2 = baseRadius * cosf(theta2);
        float z2 = baseRadius * sinf(theta2);

        // Calculate normal for this face (points outward from cone surface)
        Vector3 v1(x1, baseY, z1);
        Vector3 v2(x2, baseY, z2);
        Vector3 apex(0.0f, apexY, 0.0f);
        Vector3 edge1 = v2 - v1;
        Vector3 edge2 = apex - v1;
        Vector3 normal = edge1.cross(edge2).normalized();

        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(x1, baseY, z1);
        glVertex3f(x2, baseY, z2);
        glVertex3f(0.0f, apexY, 0.0f);
    }
    glEnd();

    glPopMatrix();
}

void Cone::tessellate(std::vector<Vector3>& positions,
                      std::vector<Vector3>& normals,
                      std::vector<int>& indices) const {
    positions.clear();
    normals.clear();
    indices.clear();

    // Base center
    positions.push_back(Vector3(0.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));

    // Base perimeter vertices
    for (int i = 0; i <= slices; i++) {
        float theta = (2.0f * M_PI * i) / slices;
        float x = baseRadius * cosf(theta);
        float z = baseRadius * sinf(theta);
        positions.push_back(Vector3(x, 0.0f, z));
        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    }

    // Base cap indices
    for (int i = 0; i < slices; i++) {
        indices.push_back(0);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
    }

    // Apex vertex
    int apexIdx = positions.size();
    positions.push_back(Vector3(0.0f, height, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));

    // Side faces
    for (int i = 0; i < slices; i++) {
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(apexIdx);
    }
}

// ========== PRISM IMPLEMENTATION ==========

Prism::Prism(Vector3 ipos, float h, float diameter, int nsides, Color icol)
    : Shape(ipos, Vector3(diameter, h, diameter), icol),
      height(h), radius(diameter / 2.0f), sides(nsides >= 3 ? nsides : 6) {
    init();
}

void Prism::init() {
    type = CYLINDER; // Use CYLINDER type for collision
}

void Prism::draw() {
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glColor3f(color.r, color.g, color.b);

    // Center the prism: bottom at -height/2, top at +height/2
    float bottomY = -height / 2.0f;
    float topY = height / 2.0f;

    // Draw bottom cap (polygon)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, bottomY, 0.0f); // Center
    for (int i = 0; i <= sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        glVertex3f(x, bottomY, z);
    }
    glEnd();

    // Draw top cap (polygon)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, topY, 0.0f); // Center
    for (int i = 0; i <= sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        glVertex3f(x, topY, z);
    }
    glEnd();

    // Draw side faces (quads)
    glBegin(GL_QUADS);
    for (int i = 0; i < sides; i++) {
        float theta1 = (2.0f * M_PI * i) / sides;
        float theta2 = (2.0f * M_PI * (i + 1)) / sides;

        float x1 = radius * cosf(theta1);
        float z1 = radius * sinf(theta1);
        float x2 = radius * cosf(theta2);
        float z2 = radius * sinf(theta2);

        // Calculate face normal (perpendicular to face)
        float midTheta = (theta1 + theta2) / 2.0f;
        Vector3 normal(cosf(midTheta), 0.0f, sinf(midTheta));

        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(x1, bottomY, z1);
        glVertex3f(x2, bottomY, z2);
        glVertex3f(x2, topY, z2);
        glVertex3f(x1, topY, z1);
    }
    glEnd();

    glPopMatrix();
}

void Prism::tessellate(std::vector<Vector3>& positions,
                       std::vector<Vector3>& normals,
                       std::vector<int>& indices) const {
    positions.clear();
    normals.clear();
    indices.clear();

    // Bottom cap vertices
    for (int i = 0; i < sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        positions.push_back(Vector3(x, 0.0f, z));
        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    }

    // Top cap vertices
    for (int i = 0; i < sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = radius * cosf(theta);
        float z = radius * sinf(theta);
        positions.push_back(Vector3(x, height, z));
        normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    }

    // Bottom cap indices
    for (int i = 1; i < sides - 1; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Top cap indices
    int topStart = sides;
    for (int i = 1; i < sides - 1; i++) {
        indices.push_back(topStart);
        indices.push_back(topStart + i + 1);
        indices.push_back(topStart + i);
    }

    // Side faces indices
    for (int i = 0; i < sides; i++) {
        int i1 = i;
        int i2 = (i + 1) % sides;
        int i3 = i + sides;
        int i4 = ((i + 1) % sides) + sides;

        // Two triangles per quad face
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);

        indices.push_back(i2);
        indices.push_back(i4);
        indices.push_back(i3);
    }
}

// ========== FRUSTUM IMPLEMENTATION ==========

Frustum::Frustum(Vector3 ipos, float h, float bottomDiameter, float topDiameter, int nsides, Color icol)
    : Shape(ipos, Vector3(bottomDiameter, h, bottomDiameter), icol),
      height(h), bottomRadius(bottomDiameter / 2.0f), topRadius(topDiameter / 2.0f),
      sides(nsides >= 3 ? nsides : 4) {
    init();
}

void Frustum::init() {
    type = CYLINDER; // Use CYLINDER type for collision
}

void Frustum::draw() {
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
    glColor3f(color.r, color.g, color.b);

    // Center the frustum: bottom at -height/2, top at +height/2
    float bottomY = -height / 2.0f;
    float topY = height / 2.0f;

    // Draw bottom cap (polygon)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(0.0f, bottomY, 0.0f); // Center
    for (int i = 0; i <= sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = bottomRadius * cosf(theta);
        float z = bottomRadius * sinf(theta);
        glVertex3f(x, bottomY, z);
    }
    glEnd();

    // Draw top cap (polygon)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, topY, 0.0f); // Center
    for (int i = 0; i <= sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = topRadius * cosf(theta);
        float z = topRadius * sinf(theta);
        glVertex3f(x, topY, z);
    }
    glEnd();

    // Draw side faces (trapezoids as quads)
    glBegin(GL_QUADS);
    for (int i = 0; i < sides; i++) {
        float theta1 = (2.0f * M_PI * i) / sides;
        float theta2 = (2.0f * M_PI * (i + 1)) / sides;

        float x1_bot = bottomRadius * cosf(theta1);
        float z1_bot = bottomRadius * sinf(theta1);
        float x2_bot = bottomRadius * cosf(theta2);
        float z2_bot = bottomRadius * sinf(theta2);

        float x1_top = topRadius * cosf(theta1);
        float z1_top = topRadius * sinf(theta1);
        float x2_top = topRadius * cosf(theta2);
        float z2_top = topRadius * sinf(theta2);

        // Calculate face normal
        Vector3 v1(x1_bot, bottomY, z1_bot);
        Vector3 v2(x2_bot, bottomY, z2_bot);
        Vector3 v3(x2_top, topY, z2_top);
        Vector3 v4(x1_top, topY, z1_top);

        Vector3 edge1 = v2 - v1;
        Vector3 edge2 = v4 - v1;
        Vector3 normal = edge1.cross(edge2).normalized();

        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(x1_bot, bottomY, z1_bot);
        glVertex3f(x2_bot, bottomY, z2_bot);
        glVertex3f(x2_top, topY, z2_top);
        glVertex3f(x1_top, topY, z1_top);
    }
    glEnd();

    glPopMatrix();
}

void Frustum::tessellate(std::vector<Vector3>& positions,
                         std::vector<Vector3>& normals,
                         std::vector<int>& indices) const {
    positions.clear();
    normals.clear();
    indices.clear();

    // Bottom cap vertices
    for (int i = 0; i < sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = bottomRadius * cosf(theta);
        float z = bottomRadius * sinf(theta);
        positions.push_back(Vector3(x, 0.0f, z));
        normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    }

    // Top cap vertices
    for (int i = 0; i < sides; i++) {
        float theta = (2.0f * M_PI * i) / sides;
        float x = topRadius * cosf(theta);
        float z = topRadius * sinf(theta);
        positions.push_back(Vector3(x, height, z));
        normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    }

    // Bottom cap indices (triangle fan)
    for (int i = 1; i < sides - 1; i++) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Top cap indices (triangle fan)
    int topStart = sides;
    for (int i = 1; i < sides - 1; i++) {
        indices.push_back(topStart);
        indices.push_back(topStart + i + 1);
        indices.push_back(topStart + i);
    }

    // Side faces indices
    for (int i = 0; i < sides; i++) {
        int i1 = i;
        int i2 = (i + 1) % sides;
        int i3 = i + sides;
        int i4 = ((i + 1) % sides) + sides;

        // Two triangles per quad face
        indices.push_back(i1);
        indices.push_back(i2);
        indices.push_back(i3);

        indices.push_back(i2);
        indices.push_back(i4);
        indices.push_back(i3);
    }
}