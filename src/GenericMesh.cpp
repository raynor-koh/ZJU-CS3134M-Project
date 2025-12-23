#include "GenericMesh.h"
#include <iostream>

GenericMesh::GenericMesh(Vector3 pos, Color col)
    : Shape(pos, Vector3(1.0f, 1.0f, 1.0f), col)
{
    type = ShapeType::GENERIC_MESH;
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
    calculateAABB();
}

void GenericMesh::setIndices(const std::vector<int>& inds) {
    indices = inds;
}

void GenericMesh::calculateAABB() {
    if (vertices.empty()) {
        size = Vector3(1.0f, 1.0f, 1.0f); // Default size if no vertices
        return;
    }

    // Find min and max bounds of all vertices
    Vector3 min = vertices[0].position;
    Vector3 max = vertices[0].position;

    for (const auto& vertex : vertices) {
        const Vector3& p = vertex.position;
        if (p.x < min.x) min.x = p.x;
        if (p.y < min.y) min.y = p.y;
        if (p.z < min.z) min.z = p.z;
        if (p.x > max.x) max.x = p.x;
        if (p.y > max.y) max.y = p.y;
        if (p.z > max.z) max.z = p.z;
    }

    // Size is the full dimensions (max - min)
    size = Vector3(max.x - min.x, max.y - min.y, max.z - min.z);

    // Add small epsilon to prevent zero-size collision boxes
    const float EPSILON = 0.1f;
    if (size.x < EPSILON) size.x = EPSILON;
    if (size.y < EPSILON) size.y = EPSILON;
    if (size.z < EPSILON) size.z = EPSILON;
}

void GenericMesh::clear() {
    vertices.clear();
    indices.clear();
}
