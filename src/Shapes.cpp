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