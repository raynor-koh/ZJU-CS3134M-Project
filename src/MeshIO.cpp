#define _USE_MATH_DEFINES
#include "MeshIO.h"
#include "GenericMesh.h"
#include "Shapes.h"
#include "Scene.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <cmath>

namespace MeshIO {

// Helper function to transform a vertex by rotation and translation
Vector3 transformVertex(const Vector3& v, const Vector3& pos,
                       const Vector3& size, const Vector3& axis, float axisAngle) {
    // Apply scale
    Vector3 scaled(v.x * size.x, v.y * size.y, v.z * size.z);

    // Apply rotation if angle is non-zero
    if (std::abs(axisAngle) > 1e-6f) {
        float angleRad = axisAngle * M_PI / 180.0f;
        float cosA = cosf(angleRad);
        float sinA = sinf(angleRad);
        Vector3 normAxis = axis.normalized();

        // Rodrigues' rotation formula
        Vector3 rotated = scaled * cosA +
                         normAxis.cross(scaled) * sinA +
                         normAxis * (normAxis.dot(scaled)) * (1.0f - cosA);
        scaled = rotated;
    }

    // Apply translation
    return Vector3(scaled.x + pos.x, scaled.y + pos.y, scaled.z + pos.z);
}

std::shared_ptr<Shape> importOBJ(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "MeshIO: Failed to open file: " << filepath << std::endl;
        return nullptr;
    }

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector3> texCoords;
    std::vector<GenericMesh::Vertex> vertices;
    std::vector<int> indices;

    std::string line;
    int lineNum = 0;

    while (std::getline(file, line)) {
        lineNum++;
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // Vertex position
            float x, y, z;
            if (iss >> x >> y >> z) {
                positions.push_back(Vector3(x, y, z));
            }
        }
        else if (prefix == "vn") {
            // Vertex normal
            float x, y, z;
            if (iss >> x >> y >> z) {
                normals.push_back(Vector3(x, y, z));
            }
        }
        else if (prefix == "vt") {
            // Texture coordinate
            float u, v;
            if (iss >> u >> v) {
                texCoords.push_back(Vector3(u, v, 0.0f));
            }
        }
        else if (prefix == "f") {
            // Face - parse various formats: v, v/vt, v//vn, v/vt/vn
            std::vector<int> faceIndices;
            std::string vertexStr;

            while (iss >> vertexStr) {
                int vIdx = 0, vtIdx = 0, vnIdx = 0;

                // Parse vertex/texture/normal indices
                size_t slash1 = vertexStr.find('/');
                if (slash1 == std::string::npos) {
                    // Format: v
                    vIdx = std::stoi(vertexStr);
                } else {
                    vIdx = std::stoi(vertexStr.substr(0, slash1));
                    size_t slash2 = vertexStr.find('/', slash1 + 1);

                    if (slash2 == std::string::npos) {
                        // Format: v/vt
                        vtIdx = std::stoi(vertexStr.substr(slash1 + 1));
                    } else {
                        // Format: v/vt/vn or v//vn
                        if (slash2 > slash1 + 1) {
                            vtIdx = std::stoi(vertexStr.substr(slash1 + 1, slash2 - slash1 - 1));
                        }
                        vnIdx = std::stoi(vertexStr.substr(slash2 + 1));
                    }
                }

                // Convert from 1-based to 0-based indexing
                vIdx = (vIdx > 0) ? vIdx - 1 : vIdx;
                vtIdx = (vtIdx > 0) ? vtIdx - 1 : vtIdx;
                vnIdx = (vnIdx > 0) ? vnIdx - 1 : vnIdx;

                // Create vertex
                GenericMesh::Vertex vertex;
                if (vIdx >= 0 && vIdx < static_cast<int>(positions.size())) {
                    vertex.position = positions[vIdx];
                }
                if (vnIdx >= 0 && vnIdx < static_cast<int>(normals.size())) {
                    vertex.normal = normals[vnIdx];
                } else {
                    vertex.normal = Vector3(0.0f, 1.0f, 0.0f);  // Default normal
                }
                if (vtIdx >= 0 && vtIdx < static_cast<int>(texCoords.size())) {
                    vertex.texCoord = texCoords[vtIdx];
                }

                // Add vertex and get index
                vertices.push_back(vertex);
                faceIndices.push_back(static_cast<int>(vertices.size()) - 1);
            }

            // Triangulate face (assumes convex polygons)
            if (faceIndices.size() >= 3) {
                // Triangle fan triangulation
                for (size_t i = 1; i < faceIndices.size() - 1; i++) {
                    indices.push_back(faceIndices[0]);
                    indices.push_back(faceIndices[i]);
                    indices.push_back(faceIndices[i + 1]);
                }
            }
        }
    }

    file.close();

    if (vertices.empty()) {
        std::cerr << "MeshIO: No geometry found in file: " << filepath << std::endl;
        return nullptr;
    }

    // Create GenericMesh and set data
    auto mesh = std::make_shared<GenericMesh>(Vector3(), Color(0.7f, 0.7f, 0.7f));
    mesh->setVertices(vertices);
    mesh->setIndices(indices);

    std::cout << "MeshIO: Loaded " << vertices.size() << " vertices, "
              << indices.size() / 3 << " triangles from " << filepath << std::endl;

    return mesh;
}

bool exportShapeOBJ(const Shape* shape, const std::string& filepath, bool applyTransform) {
    if (!shape) {
        std::cerr << "MeshIO: Cannot export null shape" << std::endl;
        return false;
    }

    // Create directory if needed
    std::filesystem::path filePath(filepath);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    // Open file for writing
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "MeshIO: Failed to create file: " << filepath << std::endl;
        return false;
    }

    // Tessellate shape
    std::vector<Vector3> positions, normals;
    std::vector<int> indices;
    shape->tessellate(positions, normals, indices);

    if (positions.empty() || indices.empty()) {
        std::cerr << "MeshIO: Shape produced no geometry" << std::endl;
        file.close();
        return false;
    }

    // Write OBJ header
    file << "# OBJ file exported from ZJU CS3134M Project\n";
    file << "# Shape type: " << shape->getType() << "\n";
    file << "# Position: " << shape->getPosition().x << " "
         << shape->getPosition().y << " " << shape->getPosition().z << "\n";
    file << "# Vertices: " << positions.size() << "\n";
    file << "# Triangles: " << indices.size() / 3 << "\n\n";

    file << "o shape_0\n\n";

    // Get transformation parameters
    Vector3 pos = shape->getPosition();
    Vector3 size = shape->getSize();
    Vector3 axis = shape->getAxis();
    float axisAngle = shape->getAxisAngle();

    // Write vertices
    for (size_t i = 0; i < positions.size(); i++) {
        Vector3 v = positions[i];

        if (applyTransform) {
            v = transformVertex(v, pos, size, axis, axisAngle);
        }

        file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }
    file << "\n";

    // Write normals
    for (size_t i = 0; i < normals.size(); i++) {
        const Vector3& n = normals[i];
        file << "vn " << n.x << " " << n.y << " " << n.z << "\n";
    }
    file << "\n";

    // Write faces (1-based indexing in OBJ format)
    for (size_t i = 0; i < indices.size(); i += 3) {
        file << "f "
             << (indices[i] + 1) << "//" << (indices[i] + 1) << " "
             << (indices[i + 1] + 1) << "//" << (indices[i + 1] + 1) << " "
             << (indices[i + 2] + 1) << "//" << (indices[i + 2] + 1) << "\n";
    }

    file.close();

    std::cout << "MeshIO: Exported shape to " << filepath << " ("
              << positions.size() << " vertices, "
              << indices.size() / 3 << " triangles)" << std::endl;

    return true;
}

bool exportSceneOBJ(const Scene* scene, const std::string& filepath) {
    if (!scene) {
        std::cerr << "MeshIO: Cannot export null scene" << std::endl;
        return false;
    }

    // Create directory if needed
    std::filesystem::path filePath(filepath);
    if (filePath.has_parent_path()) {
        std::filesystem::create_directories(filePath.parent_path());
    }

    // Open file for writing
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "MeshIO: Failed to create file: " << filepath << std::endl;
        return false;
    }

    // Write OBJ header
    file << "# OBJ file exported from ZJU CS3134M Project\n";
    file << "# Scene export\n\n";

    const auto& shapes = scene->getObjects();
    int totalVertices = 0;
    int totalTriangles = 0;
    int vertexOffset = 0;  // Track vertex offset for multi-object export

    for (size_t objIdx = 0; objIdx < shapes.size(); objIdx++) {
        const Shape* shape = shapes[objIdx].get();
        if (!shape) continue;

        // Tessellate shape
        std::vector<Vector3> positions, normals;
        std::vector<int> indices;
        shape->tessellate(positions, normals, indices);

        if (positions.empty() || indices.empty()) {
            continue;  // Skip empty shapes
        }

        // Write object group
        file << "o object_" << objIdx << "\n";
        file << "# Shape type: " << shape->getType() << "\n";
        file << "# Position: " << shape->getPosition().x << " "
             << shape->getPosition().y << " " << shape->getPosition().z << "\n\n";

        // Get transformation parameters (apply world space transform)
        Vector3 pos = shape->getPosition();
        Vector3 size = shape->getSize();
        Vector3 axis = shape->getAxis();
        float axisAngle = shape->getAxisAngle();

        // Write vertices (transformed to world space)
        for (size_t i = 0; i < positions.size(); i++) {
            Vector3 v = transformVertex(positions[i], pos, size, axis, axisAngle);
            file << "v " << v.x << " " << v.y << " " << v.z << "\n";
        }
        file << "\n";

        // Write normals
        for (size_t i = 0; i < normals.size(); i++) {
            const Vector3& n = normals[i];
            file << "vn " << n.x << " " << n.y << " " << n.z << "\n";
        }
        file << "\n";

        // Write faces (adjust indices by vertex offset, 1-based indexing)
        for (size_t i = 0; i < indices.size(); i += 3) {
            int idx0 = indices[i] + vertexOffset + 1;
            int idx1 = indices[i + 1] + vertexOffset + 1;
            int idx2 = indices[i + 2] + vertexOffset + 1;

            file << "f "
                 << idx0 << "//" << idx0 << " "
                 << idx1 << "//" << idx1 << " "
                 << idx2 << "//" << idx2 << "\n";
        }
        file << "\n";

        // Update offset and stats
        vertexOffset += static_cast<int>(positions.size());
        totalVertices += static_cast<int>(positions.size());
        totalTriangles += static_cast<int>(indices.size()) / 3;
    }

    file.close();

    std::cout << "MeshIO: Exported scene to " << filepath << " ("
              << shapes.size() << " objects, "
              << totalVertices << " vertices, "
              << totalTriangles << " triangles)" << std::endl;

    return true;
}

}  // namespace MeshIO
