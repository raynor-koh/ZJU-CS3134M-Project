#pragma once
#include "Shapes.h"
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>

class GenericMesh : public Shape {
public:
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector3 texCoord;  // (u, v, 0) for future use

        Vertex() : position(), normal(), texCoord() {}
        Vertex(Vector3 pos, Vector3 norm, Vector3 tex = Vector3())
            : position(pos), normal(norm), texCoord(tex) {}
    };

    GenericMesh(Vector3 pos = Vector3(), Color col = Color());
    virtual ~GenericMesh();

    // Render the mesh using immediate mode
    void draw() override;

    // Set mesh data
    void setVertices(const std::vector<Vertex>& verts);
    void setIndices(const std::vector<int>& inds);

    // Get mesh data
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<int>& getIndices() const { return indices; }

    // Calculate and update AABB from vertex data
    void calculateAABB();

    // Clear mesh data
    void clear();

private:
    std::vector<Vertex> vertices;
    std::vector<int> indices;  // Triangle list
};
