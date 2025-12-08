#pragma once

struct Color {
    float r, g, b;
    Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) : r(r), g(g), b(b) {}
};

struct Vector3 {
    float x, y, z;
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
};

// Collision types for different object behaviors
enum class CollisionType {
    SOLID,      // Normal solid objects (walls, boxes)
    CLIMBABLE,  // Objects that can be climbed (ladders) - for future use
    WALKABLE,   // Objects you can walk on (stairs, platforms) - for future use
    NONE        // No collision
};

class GameObject {
public:
    GameObject(const Vector3& position, const Vector3& size, const Color& color);
    virtual ~GameObject() = default;  // Make GameObject polymorphic

    virtual void draw() const;  // Make draw virtual so derived classes can override

    // Getters
    Vector3 getPosition() const { return position; }
    Vector3 getSize() const { return size; }
    Color getColor() const { return color; }

    // Setters
    void setPosition(const Vector3& pos) { position = pos; }
    void setSize(const Vector3& sz) { size = sz; }
    void setColor(const Color& col) { color = col; }
    void setCollisionType(CollisionType type) { collisionType = type; }

    // Collision
    CollisionType getCollisionType() const { return collisionType; }
    virtual bool checkAABBCollision(float x, float z, float radius) const;

private:
    Vector3 position;
    Vector3 size;
    Color color;
    CollisionType collisionType;
};
