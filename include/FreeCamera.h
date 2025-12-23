#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include "GameObject.h"

// Free Camera / Spectator Mode (Photo Mode)
// Game-style detached camera for scene exploration
// Controls: Mouse wheel (zoom), Alt+drag (orbit/pan), F (zoom-to-fit)
class FreeCamera {
public:
    FreeCamera(float startX, float startY, float startZ);
    ~FreeCamera();

    // Apply camera transformation to OpenGL
    void applyView();

    // Camera movement controls
    void orbit(float deltaAzimuth, float deltaElevation);
    void pan(float deltaX, float deltaY);
    void zoom(float delta);
    void zoomToFit(const Vector3& sceneCenter, float sceneRadius);

    // Camera state
    void setOrbitTarget(const Vector3& target);
    void setPosition(float x, float y, float z);
    Vector3 getPosition() const;
    Vector3 getOrbitTarget() const { return orbitTarget; }

private:
    // Orbit mode: spherical coordinates around target
    Vector3 orbitTarget;  // Point camera orbits around
    float azimuth;        // Horizontal angle (radians)
    float elevation;      // Vertical angle (radians)
    float distance;       // Distance from orbit target

    // Update camera position from spherical coordinates
    void updatePosition();
};
