#pragma once
#include <GL/glew.h>
#include <GL/glut.h>

class Lighting {
public:
    Lighting();
    ~Lighting();

    // Apply lighting state to OpenGL (call at start of each frame)
    void apply();

    // Update light position to follow camera (headlight mode)
    void updateHeadlight(float camX, float camY, float camZ, float lookX, float lookY, float lookZ);

    // Enable/disable lighting
    void setEnabled(bool enabled);
    bool isEnabled() const { return enabled; }
    void toggle() { enabled = !enabled; }

    // Light position control (world space)
    void setPosition(float x, float y, float z);
    void adjustPosition(float dx, float dy, float dz);
    void getPosition(float& x, float& y, float& z) const;

    // Light intensity control (multiplier for diffuse/specular)
    void setIntensity(float intensity);
    void adjustIntensity(float delta);
    float getIntensity() const { return intensity; }

    // Light type: directional (w=0) vs point light (w=1)
    void setLightType(bool isPointLight);
    bool isPointLight() const { return pointLight; }
    void toggleLightType() { pointLight = !pointLight; }

    // Headlight mode (light follows camera)
    void setHeadlightMode(bool enable) { headlightMode = enable; }
    bool isHeadlightMode() const { return headlightMode; }

    // Attenuation parameters (for point lights)
    void setAttenuation(float constant, float linear, float quadratic);

    // Print current lighting state to console
    void printStatus() const;

private:
    bool enabled;
    bool pointLight;  // true = point light (w=1), false = directional (w=0)
    bool headlightMode;  // true = light follows camera (updated each frame)

    // Light position/direction
    float posX, posY, posZ;

    // Light intensity (scalar multiplier)
    float intensity;

    // Light colors (base values, multiplied by intensity when applied)
    float ambientColor[4];
    float diffuseColor[4];
    float specularColor[4];

    // Attenuation (for point lights)
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;

    // Update OpenGL light parameters
    void updateLight();
};
