#include "Lighting.h"
#include <iostream>
#include <iomanip>

Lighting::Lighting()
    : enabled(true),  // Start with lighting ENABLED for realistic look
      pointLight(true),
      headlightMode(true),  // Default to headlight mode (follows camera)
      posX(0.0f), posY(0.0f), posZ(0.0f),  // Will be updated by headlight mode
      intensity(1.0f),
      constantAttenuation(1.0f),
      linearAttenuation(0.0f),  // No attenuation for headlight
      quadraticAttenuation(0.0f)
{
    // Ambient light (low intensity for some base illumination)
    ambientColor[0] = 0.2f;
    ambientColor[1] = 0.2f;
    ambientColor[2] = 0.2f;
    ambientColor[3] = 1.0f;

    // Diffuse light (main illumination, will be scaled by intensity)
    diffuseColor[0] = 1.0f;
    diffuseColor[1] = 1.0f;
    diffuseColor[2] = 1.0f;
    diffuseColor[3] = 1.0f;

    // Specular light (highlights)
    specularColor[0] = 1.0f;
    specularColor[1] = 1.0f;
    specularColor[2] = 1.0f;
    specularColor[3] = 1.0f;
}

Lighting::~Lighting() {
    // Cleanup if needed
}

void Lighting::apply() {
    if (enabled) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        // Enable automatic normalization (in case of scaling)
        glEnable(GL_NORMALIZE);

        // Enable color material so existing glColor calls affect material
        glEnable(GL_COLOR_MATERIAL);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

        // Set default material properties
        GLfloat matSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GLfloat matShininess[] = { 32.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

        // Update light parameters
        updateLight();
    } else {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_NORMALIZE);
        glDisable(GL_COLOR_MATERIAL);
    }
}

void Lighting::updateLight() {
    // Position (4th component: w=0 for directional, w=1 for point)
    GLfloat position[4] = {
        posX,
        posY,
        posZ,
        pointLight ? 1.0f : 0.0f
    };
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    // Ambient (not scaled by intensity to maintain minimum illumination)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);

    // Diffuse (scaled by intensity)
    GLfloat scaledDiffuse[4] = {
        diffuseColor[0] * intensity,
        diffuseColor[1] * intensity,
        diffuseColor[2] * intensity,
        diffuseColor[3]
    };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, scaledDiffuse);

    // Specular (scaled by intensity)
    GLfloat scaledSpecular[4] = {
        specularColor[0] * intensity,
        specularColor[1] * intensity,
        specularColor[2] * intensity,
        specularColor[3]
    };
    glLightfv(GL_LIGHT0, GL_SPECULAR, scaledSpecular);

    // Attenuation (only for point lights)
    if (pointLight) {
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, constantAttenuation);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, linearAttenuation);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);
    } else {
        // Directional light - no attenuation
        glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
        glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);
    }
}

void Lighting::setEnabled(bool enable) {
    enabled = enable;
    if (enabled) {
        std::cout << "Lighting ENABLED" << std::endl;
        printStatus();
    } else {
        std::cout << "Lighting DISABLED" << std::endl;
    }
}

void Lighting::setPosition(float x, float y, float z) {
    posX = x;
    posY = y;
    posZ = z;
}

void Lighting::adjustPosition(float dx, float dy, float dz) {
    posX += dx;
    posY += dy;
    posZ += dz;
    if (enabled) {
        std::cout << std::fixed << std::setprecision(1)
                  << "Light position: (" << posX << ", " << posY << ", " << posZ << ")" << std::endl;
    }
}

void Lighting::getPosition(float& x, float& y, float& z) const {
    x = posX;
    y = posY;
    z = posZ;
}

void Lighting::setIntensity(float inten) {
    intensity = inten;
    if (intensity < 0.0f) intensity = 0.0f;
    if (intensity > 5.0f) intensity = 5.0f;  // Cap at 5x for safety
}

void Lighting::adjustIntensity(float delta) {
    intensity += delta;
    if (intensity < 0.0f) intensity = 0.0f;
    if (intensity > 5.0f) intensity = 5.0f;
    if (enabled) {
        std::cout << std::fixed << std::setprecision(2)
                  << "Light intensity: " << intensity << std::endl;
    }
}

void Lighting::setLightType(bool isPoint) {
    pointLight = isPoint;
    if (enabled) {
        std::cout << "Light type: " << (pointLight ? "Point Light" : "Directional Light") << std::endl;
    }
}

void Lighting::setAttenuation(float constant, float linear, float quadratic) {
    constantAttenuation = constant;
    linearAttenuation = linear;
    quadraticAttenuation = quadratic;
}

void Lighting::updateHeadlight(float camX, float camY, float camZ, float lookX, float lookY, float lookZ) {
    if (!headlightMode) return;

    // Position light slightly in front of camera along look direction
    // This creates a headlight/flashlight effect
    float offset = 0.5f;  // Distance in front of camera
    posX = camX + lookX * offset;
    posY = camY + lookY * offset;
    posZ = camZ + lookZ * offset;
}

void Lighting::printStatus() const {
    std::cout << "=== Lighting Status ===" << std::endl;
    std::cout << "  Mode: " << (headlightMode ? "Headlight (follows camera)" : "Static") << std::endl;
    std::cout << "  Type: " << (pointLight ? "Point Light" : "Directional Light") << std::endl;
    std::cout << std::fixed << std::setprecision(1)
              << "  Position: (" << posX << ", " << posY << ", " << posZ << ")" << std::endl;
    std::cout << std::fixed << std::setprecision(2)
              << "  Intensity: " << intensity << std::endl;
    if (pointLight) {
        std::cout << "  Attenuation: const=" << constantAttenuation
                  << " linear=" << linearAttenuation
                  << " quad=" << quadraticAttenuation << std::endl;
    }
    std::cout << "=======================" << std::endl;
}
