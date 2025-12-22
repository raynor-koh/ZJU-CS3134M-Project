#include "GenericMesh.h"
#include <iostream>

GenericMesh::GenericMesh(Vector3 pos, Color col)
    : Shape(pos, Vector3(1.0f, 1.0f, 1.0f), col)
{
    type = ShapeType::CUBE; // Use CUBE as default type for generic mesh
}

GenericMesh::~GenericMesh() {
    // Cleanup if needed
}

void GenericMesh::draw() {
    if (vertices.empty() || indices.empty()) {
        return; // Nothing to draw
    }

    glPushMatrix();

    // Apply transformations
    glTranslatef(pos.x, pos.y, pos.z);

    // Apply rotation if axis angle is non-zero
    if (axisAngle != 0.0f) {
        glRotatef(axisAngle, axis.x, axis.y, axis.z);
    }

    // Apply scale
    glScalef(size.x, size.y, size.z);

    // Set material color
    glColor3f(color.r, color.g, color.b);

    // Draw mesh using immediate mode (compatible with existing codebase)
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); i++) {
        int idx = indices[i];
        if (idx >= 0 && idx < static_cast<int>(vertices.size())) {
            const Vertex& v = vertices[idx];
            glNormal3f(v.normal.x, v.normal.y, v.normal.z);
            glVertex3f(v.position.x, v.position.y, v.position.z);
        }
    }
    glEnd();

    glPopMatrix();
}

void GenericMesh::setVertices(const std::vector<Vertex>& verts) {
    vertices = verts;
}

void GenericMesh::setIndices(const std::vector<int>& inds) {
    indices = inds;
}

void GenericMesh::clear() {
    vertices.clear();
    indices.clear();
}
