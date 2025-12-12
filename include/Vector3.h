#pragma once

#include <cmath>

struct Vector3 {
    float x, y, z;
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }
    
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    float length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
    
    Vector3 normalized() const {
        float len = length();
        if (len < 1e-12) return *this;
        return *this / len;
    }
};


struct Color {
    float r, g, b;
    Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) : r(r), g(g), b(b) {}
};