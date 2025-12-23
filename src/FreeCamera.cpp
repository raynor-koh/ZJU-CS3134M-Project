#include "FreeCamera.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FreeCamera::FreeCamera(float startX, float startY, float startZ)
    : orbitTarget(0.0f, 0.0f, 0.0f)
    , azimuth(0.0f)
    , elevation(M_PI / 6.0f)  // Start at 30 degrees
    , distance(20.0f)
{
    // Initialize to look at origin from starting position
    orbitTarget = Vector3(0.0f, 2.0f, 0.0f);  // Scene center default
    updatePosition();
}

FreeCamera::~FreeCamera() {
}

void FreeCamera::applyView() {
    // Compute camera position from spherical coordinates
    float camX = orbitTarget.x + distance * cos(elevation) * sin(azimuth);
    float camY = orbitTarget.y + distance * sin(elevation);
    float camZ = orbitTarget.z + distance * cos(elevation) * cos(azimuth);

    // Look at orbit target
    gluLookAt(
        camX, camY, camZ,           // Eye position
        orbitTarget.x, orbitTarget.y, orbitTarget.z,  // Look at point
        0.0f, 1.0f, 0.0f            // Up vector
    );
}

void FreeCamera::orbit(float deltaAzimuth, float deltaElevation) {
    azimuth += deltaAzimuth;
    elevation += deltaElevation;

    // Clamp elevation to avoid gimbal lock
    const float maxElevation = M_PI / 2.0f - 0.01f;  // Just below 90 degrees
    const float minElevation = -M_PI / 2.0f + 0.01f; // Just above -90 degrees
    elevation = std::max(minElevation, std::min(maxElevation, elevation));

    // Wrap azimuth to [0, 2*PI]
    while (azimuth < 0.0f) azimuth += 2.0f * M_PI;
    while (azimuth >= 2.0f * M_PI) azimuth -= 2.0f * M_PI;

    updatePosition();
}

void FreeCamera::pan(float deltaX, float deltaY) {
    // Compute camera's right and up vectors
    float rightX = cos(azimuth);
    float rightZ = -sin(azimuth);

    float upX = -sin(elevation) * sin(azimuth);
    float upY = cos(elevation);
    float upZ = -sin(elevation) * cos(azimuth);

    // Move orbit target (camera follows)
    orbitTarget.x += rightX * deltaX + upX * deltaY;
    orbitTarget.y += upY * deltaY;
    orbitTarget.z += rightZ * deltaX + upZ * deltaY;

    updatePosition();
}

void FreeCamera::zoom(float delta) {
    distance -= delta;

    // Clamp distance to reasonable range
    const float minDistance = 1.0f;
    const float maxDistance = 200.0f;
    distance = std::max(minDistance, std::min(maxDistance, distance));

    updatePosition();
}

void FreeCamera::zoomToFit(const Vector3& sceneCenter, float sceneRadius) {
    orbitTarget = sceneCenter;

    // Calculate distance needed to fit scene in view
    // Assume 45-degree FOV (default perspective FOV)
    const float fov = 45.0f * M_PI / 180.0f;
    const float requiredDistance = sceneRadius / sin(fov / 2.0f);

    distance = requiredDistance * 1.2f;  // Add 20% padding

    // Reset camera to a good default angle
    azimuth = M_PI / 4.0f;     // 45 degrees
    elevation = M_PI / 6.0f;   // 30 degrees

    updatePosition();
}

void FreeCamera::setOrbitTarget(const Vector3& target) {
    orbitTarget = target;
    updatePosition();
}

void FreeCamera::setPosition(float x, float y, float z) {
    // Convert position to spherical coordinates around current target
    Vector3 toCamera(x - orbitTarget.x, y - orbitTarget.y, z - orbitTarget.z);
    distance = sqrt(toCamera.x * toCamera.x + toCamera.y * toCamera.y + toCamera.z * toCamera.z);

    if (distance > 0.01f) {
        azimuth = atan2(toCamera.x, toCamera.z);
        elevation = asin(toCamera.y / distance);
    }

    updatePosition();
}

Vector3 FreeCamera::getPosition() const {
    float camX = orbitTarget.x + distance * cos(elevation) * sin(azimuth);
    float camY = orbitTarget.y + distance * sin(elevation);
    float camZ = orbitTarget.z + distance * cos(elevation) * cos(azimuth);
    return Vector3(camX, camY, camZ);
}

void FreeCamera::updatePosition() {
    // Position is computed dynamically in applyView()
    // This function exists for future extensibility
}
