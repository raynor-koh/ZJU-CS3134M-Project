#pragma once
#include <string>
#include <memory>

// Forward declarations
class Shape;
class Scene;

namespace MeshIO {
    // Import OBJ file and return a GenericMesh shape
    // Returns nullptr on failure
    std::shared_ptr<Shape> importOBJ(const std::string& filepath);

    // Export a single shape to OBJ format
    // applyTransform: if true, applies position/rotation/scale to vertices (world space)
    //                 if false, exports in local space
    // Returns true on success, false on failure
    bool exportShapeOBJ(const Shape* shape, const std::string& filepath,
                       bool applyTransform = true);

    // Export entire scene to OBJ format
    // All shapes exported in world space with object groups
    // Returns true on success, false on failure
    bool exportSceneOBJ(const Scene* scene, const std::string& filepath);
}
