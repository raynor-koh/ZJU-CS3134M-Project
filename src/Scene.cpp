#define _USE_MATH_DEFINES
#include "Scene.h"
#include "Bullet.h"
#include "Target.h"
#include "Shapes.h"
#include "Enemy.h"
#include "CollisionDetector.h"
#include "Lighting.h"
#include <GL/glut.h>
#include <cmath>

Scene::Scene()
    : groundSize(50.0f), groundColor(0.2f, 0.6f, 0.2f), wallHeight(5.0f), wallThickness(1.0f), lighting(nullptr),
      playerInsideSafeZone(false) {
}

Scene::~Scene() {
    clearGameObjects();
    clearBullets();
    // Note: targets are managed separately and should be deleted by the caller
    targets.clear();
}

void Scene::initialize() {
    // Load textures
    addTexture(new Texture("resources/textures/WoodCap.bmp"));
    addTexture(new Texture("resources/textures/WoodSide.bmp"));

    glassPanels.clear();

    // Add obstacles - SPREAD OUT to allow enemy navigation (min 8 units apart)
    // Note: house is intentionally placed at origin inside the safe zone.

    // Tree (trunk + canopy) at (-18, 12)
    addShape(std::make_shared<Cylinder>(Vector3(-18.0f, 1.5f, 12.0f), 3.0f, 0.8f, Color(0.5f, 0.3f, 0.15f)));
    addShape(std::make_shared<Cone>(Vector3(-18.0f, 4.5f, 12.0f), 3.0f, 3.0f, Color(0.1f, 0.6f, 0.2f)));

    // Lamp post at (0, -25)
    addShape(std::make_shared<Cylinder>(Vector3(0.0f, 2.0f, -25.0f), 4.0f, 0.4f, Color(0.2f, 0.2f, 0.2f)));
    addShape(std::make_shared<Sphere>(Vector3(0.0f, 4.2f, -25.0f), 1.0f, Color(1.0f, 0.95f, 0.6f)));

    // Boulder cluster at (30, 0)
    addShape(std::make_shared<Sphere>(Vector3(30.0f, 1.4f, 0.0f), 2.8f, Color(0.45f, 0.45f, 0.5f)));
    addShape(std::make_shared<Sphere>(Vector3(31.5f, 0.7f, -1.5f), 1.4f, Color(0.4f, 0.4f, 0.45f)));
    addShape(std::make_shared<Sphere>(Vector3(28.5f, 0.7f, 1.2f), 1.2f, Color(0.4f, 0.4f, 0.45f)));

    // Crate stack at (15, 15)
    auto crate1 = std::make_shared<Cube>(Vector3(15.0f, 1.0f, 15.0f), Vector3(2.0f, 2.0f, 2.0f), Color(0.7f, 0.4f, 0.2f));
    auto crate2 = std::make_shared<Cube>(Vector3(15.8f, 3.0f, 15.2f), Vector3(1.6f, 2.0f, 1.6f), Color(0.7f, 0.4f, 0.2f));
    addShape(crate1);
    addShape(crate2);
    crate1->bindTexture(textures[1], Shape::BOTH);
    crate2->bindTexture(textures[1], Shape::BOTH);

    // Silo at (20, -15)
    addShape(std::make_shared<Cylinder>(Vector3(20.0f, 2.5f, -15.0f), 5.0f, 2.0f, Color(0.7f, 0.7f, 0.7f)));
    addShape(std::make_shared<Cone>(Vector3(20.0f, 5.5f, -15.0f), 2.0f, 2.6f, Color(0.5f, 0.5f, 0.5f)));

    // Bench at (-22, -20)
    addShape(std::make_shared<Cube>(Vector3(-22.0f, 0.8f, -20.0f), Vector3(3.0f, 0.4f, 1.0f), Color(0.4f, 0.25f, 0.15f)));
    addShape(std::make_shared<Cube>(Vector3(-23.2f, 0.4f, -20.3f), Vector3(0.4f, 0.8f, 0.4f), Color(0.35f, 0.2f, 0.1f)));
    addShape(std::make_shared<Cube>(Vector3(-20.8f, 0.4f, -19.7f), Vector3(0.4f, 0.8f, 0.4f), Color(0.35f, 0.2f, 0.1f)));

    // Signpost at (-30, 25)
    addShape(std::make_shared<Cylinder>(Vector3(-30.0f, 1.5f, 25.0f), 3.0f, 0.4f, Color(0.4f, 0.3f, 0.2f)));
    addShape(std::make_shared<Cube>(Vector3(-30.0f, 2.4f, 25.8f), Vector3(2.0f, 0.8f, 0.2f), Color(0.9f, 0.9f, 0.7f)));

    // Barn/warehouse at (0, 35)
    auto barn = std::make_shared<Cube>(Vector3(0.0f, 4.0f, 35.0f), Vector3(6.0f, 8.0f, 6.0f), Color(0.85f, 0.45f, 0.2f));
    addShape(barn);
    barn->bindTexture(textures[1], Shape::BOTH);
    addShape(std::make_shared<Cylinder>(Vector3(2.5f, 7.0f, 33.5f), 2.0f, 0.6f, Color(0.2f, 0.2f, 0.2f)));

    // Shed at (-35, -35)
    addShape(std::make_shared<Cube>(Vector3(-35.0f, 1.0f, -35.0f), Vector3(3.0f, 2.0f, 3.0f), Color(0.5f, 0.35f, 0.2f)));
    addShape(std::make_shared<Cone>(Vector3(-35.0f, 2.8f, -35.0f), 2.0f, 4.0f, Color(0.6f, 0.1f, 0.1f)));

    // Water tower at (38, 22)
    addShape(std::make_shared<Cylinder>(Vector3(38.0f, 3.0f, 22.0f), 6.0f, 1.0f, Color(0.6f, 0.6f, 0.7f)));
    addShape(std::make_shared<Sphere>(Vector3(38.0f, 6.5f, 22.0f), 2.6f, Color(0.55f, 0.55f, 0.6f)));

    // Barrel stack at (34, -28)
    addShape(std::make_shared<Cylinder>(Vector3(34.0f, 1.0f, -28.0f), 2.0f, 1.4f, Color(0.55f, 0.3f, 0.1f)));
    addShape(std::make_shared<Cylinder>(Vector3(35.6f, 1.0f, -28.6f), 2.0f, 1.4f, Color(0.55f, 0.3f, 0.1f)));

    // Tent at (-10, 40)
    addShape(std::make_shared<Cube>(Vector3(-10.0f, 0.4f, 40.0f), Vector3(3.8f, 0.6f, 3.8f), Color(0.3f, 0.3f, 0.35f)));
    addShape(std::make_shared<Cone>(Vector3(-10.0f, 1.8f, 40.0f), 3.0f, 4.0f, Color(0.8f, 0.8f, 0.2f)));

    // Fountain at (12, -38)
    addShape(std::make_shared<Cylinder>(Vector3(12.0f, 0.4f, -38.0f), 0.8f, 4.0f, Color(0.5f, 0.5f, 0.55f)));
    addShape(std::make_shared<Cylinder>(Vector3(12.0f, 1.2f, -38.0f), 1.6f, 2.0f, Color(0.6f, 0.6f, 0.65f)));
    addShape(std::make_shared<Sphere>(Vector3(12.0f, 2.2f, -38.0f), 1.2f, Color(0.2f, 0.6f, 0.9f)));

    // Windmill at (42, -8)
    addShape(std::make_shared<Cylinder>(Vector3(42.0f, 3.0f, -8.0f), 6.0f, 1.2f, Color(0.8f, 0.8f, 0.8f)));
    addShape(std::make_shared<Cone>(Vector3(42.0f, 7.0f, -8.0f), 2.0f, 2.6f, Color(0.7f, 0.7f, 0.7f)));
    addShape(std::make_shared<Cube>(Vector3(42.0f, 6.2f, -8.0f), Vector3(4.0f, 0.2f, 0.4f), Color(0.95f, 0.95f, 0.95f)));
    addShape(std::make_shared<Cube>(Vector3(42.0f, 6.2f, -8.0f), Vector3(0.4f, 0.2f, 4.0f), Color(0.95f, 0.95f, 0.95f)));

    // Parked car at (-40, -5)
    addShape(std::make_shared<Cube>(Vector3(-40.0f, 0.6f, -5.0f), Vector3(4.0f, 1.2f, 2.0f), Color(0.2f, 0.4f, 0.8f)));
    addShape(std::make_shared<Cube>(Vector3(-39.8f, 1.4f, -5.0f), Vector3(2.4f, 1.0f, 1.6f), Color(0.25f, 0.45f, 0.85f)));
    addShape(std::make_shared<Sphere>(Vector3(-41.6f, 0.3f, -5.8f), 0.6f, Color(0.1f, 0.1f, 0.1f)));
    addShape(std::make_shared<Sphere>(Vector3(-41.6f, 0.3f, -4.2f), 0.6f, Color(0.1f, 0.1f, 0.1f)));
    addShape(std::make_shared<Sphere>(Vector3(-38.4f, 0.3f, -5.8f), 0.6f, Color(0.1f, 0.1f, 0.1f)));
    addShape(std::make_shared<Sphere>(Vector3(-38.4f, 0.3f, -4.2f), 0.6f, Color(0.1f, 0.1f, 0.1f)));

    // Stone arch at (8, 22)
    addShape(std::make_shared<Cube>(Vector3(7.4f, 1.5f, 22.0f), Vector3(0.8f, 3.0f, 0.8f), Color(0.6f, 0.6f, 0.6f)));
    addShape(std::make_shared<Cube>(Vector3(10.6f, 1.5f, 22.0f), Vector3(0.8f, 3.0f, 0.8f), Color(0.6f, 0.6f, 0.6f)));
    addShape(std::make_shared<Cube>(Vector3(9.0f, 3.4f, 22.0f), Vector3(3.6f, 0.8f, 0.8f), Color(0.6f, 0.6f, 0.6f)));

    // Campfire ring at (-18, 42)
    addShape(std::make_shared<Cylinder>(Vector3(-18.0f, 0.2f, 42.0f), 0.4f, 1.8f, Color(0.3f, 0.3f, 0.3f)));
    addShape(std::make_shared<Cone>(Vector3(-18.0f, 0.9f, 42.0f), 1.0f, 1.0f, Color(0.9f, 0.4f, 0.1f)));

    // House with door and windows at safe zone center
    const Vector3 houseCenter(0.0f, 0.0f, 0.0f);
    const float houseWidth = 4.2f;
    const float houseDepth = 3.6f;
    const float wallHeight = 2.6f;
    const float wallThickness = 0.4f;
    const float doorWidth = 1.4f;
    const float doorHeight = 1.9f;
    const float frontZ = houseCenter.z + houseDepth * 0.5f;
    const float backZ = houseCenter.z - houseDepth * 0.5f;
    const float leftX = houseCenter.x - houseWidth * 0.5f;
    const float rightX = houseCenter.x + houseWidth * 0.5f;
    Color wallColor(0.75f, 0.75f, 0.7f);
    Color trimColor(0.6f, 0.55f, 0.5f);
    Color doorColor(0.4f, 0.25f, 0.15f);

    // Back wall
    addShape(std::make_shared<Cube>(Vector3(houseCenter.x, wallHeight * 0.5f, backZ),
                                    Vector3(houseWidth, wallHeight, wallThickness), wallColor));

    // Side walls with window openings
    const float windowWidth = 1.2f;
    const float windowHeight = 0.8f;
    const float windowBottom = 1.0f;
    const float windowTop = windowBottom + windowHeight;

    const float leftWindowCenterZ = houseCenter.z + 0.6f;
    const float rightWindowCenterZ = houseCenter.z - 0.6f;
    const float leftWindowStartZ = leftWindowCenterZ - windowWidth * 0.5f;
    const float leftWindowEndZ = leftWindowCenterZ + windowWidth * 0.5f;
    const float rightWindowStartZ = rightWindowCenterZ - windowWidth * 0.5f;
    const float rightWindowEndZ = rightWindowCenterZ + windowWidth * 0.5f;

    // Left wall segments (front/back) full height
    addShape(std::make_shared<Cube>(Vector3(leftX, wallHeight * 0.5f, (backZ + leftWindowStartZ) * 0.5f),
                                    Vector3(wallThickness, wallHeight, leftWindowStartZ - backZ), wallColor));
    addShape(std::make_shared<Cube>(Vector3(leftX, wallHeight * 0.5f, (leftWindowEndZ + frontZ) * 0.5f),
                                    Vector3(wallThickness, wallHeight, frontZ - leftWindowEndZ), wallColor));
    // Left wall bands (below/above window)
    addShape(std::make_shared<Cube>(Vector3(leftX, windowBottom * 0.5f, leftWindowCenterZ),
                                    Vector3(wallThickness, windowBottom, windowWidth), wallColor));
    addShape(std::make_shared<Cube>(Vector3(leftX, windowTop + (wallHeight - windowTop) * 0.5f, leftWindowCenterZ),
                                    Vector3(wallThickness, wallHeight - windowTop, windowWidth), wallColor));

    // Right wall segments (front/back) full height
    addShape(std::make_shared<Cube>(Vector3(rightX, wallHeight * 0.5f, (backZ + rightWindowStartZ) * 0.5f),
                                    Vector3(wallThickness, wallHeight, rightWindowStartZ - backZ), wallColor));
    addShape(std::make_shared<Cube>(Vector3(rightX, wallHeight * 0.5f, (rightWindowEndZ + frontZ) * 0.5f),
                                    Vector3(wallThickness, wallHeight, frontZ - rightWindowEndZ), wallColor));
    // Right wall bands (below/above window)
    addShape(std::make_shared<Cube>(Vector3(rightX, windowBottom * 0.5f, rightWindowCenterZ),
                                    Vector3(wallThickness, windowBottom, windowWidth), wallColor));
    addShape(std::make_shared<Cube>(Vector3(rightX, windowTop + (wallHeight - windowTop) * 0.5f, rightWindowCenterZ),
                                    Vector3(wallThickness, wallHeight - windowTop, windowWidth), wallColor));

    // Front wall with door opening
    const float frontSegmentWidth = (houseWidth - doorWidth) * 0.5f;
    addShape(std::make_shared<Cube>(Vector3(houseCenter.x - (doorWidth * 0.5f + frontSegmentWidth * 0.5f),
                                            wallHeight * 0.5f, frontZ),
                                    Vector3(frontSegmentWidth, wallHeight, wallThickness), wallColor));
    addShape(std::make_shared<Cube>(Vector3(houseCenter.x + (doorWidth * 0.5f + frontSegmentWidth * 0.5f),
                                            wallHeight * 0.5f, frontZ),
                                    Vector3(frontSegmentWidth, wallHeight, wallThickness), wallColor));
    addShape(std::make_shared<Cube>(Vector3(houseCenter.x, doorHeight + (wallHeight - doorHeight) * 0.5f, frontZ),
                                    Vector3(doorWidth, wallHeight - doorHeight, wallThickness), trimColor));
    // Door opening is left clear for entry (no door geometry)

    // Roof
    addShape(std::make_shared<Cube>(Vector3(houseCenter.x, wallHeight + 0.25f, houseCenter.z),
                                    Vector3(houseWidth + 0.8f, 0.5f, houseDepth + 0.8f), trimColor));

    // Window frames (left/right walls) - thin bars so glass stays see-through
    const float frameThickness = 0.12f;
    const float frameDepth = windowWidth + 0.4f;
    const float frameHeight = windowHeight + 0.4f;
    const float leftFrameX = leftX - 0.18f;
    const float rightFrameX = rightX + 0.18f;
    const float frameCenterY = windowBottom + windowHeight * 0.5f;
    const float topY = windowBottom + windowHeight;

    // Left window frame bars
    addShape(std::make_shared<Cube>(Vector3(leftFrameX, windowBottom + 0.05f, leftWindowCenterZ),
                                    Vector3(frameThickness, 0.1f, frameDepth), trimColor));
    addShape(std::make_shared<Cube>(Vector3(leftFrameX, topY - 0.05f, leftWindowCenterZ),
                                    Vector3(frameThickness, 0.1f, frameDepth), trimColor));
    addShape(std::make_shared<Cube>(Vector3(leftFrameX, frameCenterY, leftWindowStartZ),
                                    Vector3(frameThickness, frameHeight, 0.1f), trimColor));
    addShape(std::make_shared<Cube>(Vector3(leftFrameX, frameCenterY, leftWindowEndZ),
                                    Vector3(frameThickness, frameHeight, 0.1f), trimColor));

    // Right window frame bars
    addShape(std::make_shared<Cube>(Vector3(rightFrameX, windowBottom + 0.05f, rightWindowCenterZ),
                                    Vector3(frameThickness, 0.1f, frameDepth), trimColor));
    addShape(std::make_shared<Cube>(Vector3(rightFrameX, topY - 0.05f, rightWindowCenterZ),
                                    Vector3(frameThickness, 0.1f, frameDepth), trimColor));
    addShape(std::make_shared<Cube>(Vector3(rightFrameX, frameCenterY, rightWindowStartZ),
                                    Vector3(frameThickness, frameHeight, 0.1f), trimColor));
    addShape(std::make_shared<Cube>(Vector3(rightFrameX, frameCenterY, rightWindowEndZ),
                                    Vector3(frameThickness, frameHeight, 0.1f), trimColor));

    // Glass panels for windows (translucent material effect)
    addGlassPanel(Vector3(leftX - 0.02f, windowBottom + windowHeight * 0.5f, leftWindowCenterZ),
                  windowWidth, windowHeight, true, -1.0f);
    addGlassPanel(Vector3(rightX + 0.02f, windowBottom + windowHeight * 0.5f, rightWindowCenterZ),
                  windowWidth, windowHeight, true, 1.0f);

    // Initialize the grids after all obstacles are added
    rebuildCollisionGrid();
    rebuildNavigationGrid();
}

void Scene::draw() const {
    // NOTE: Lighting is applied in display() AFTER camera view is set
    // This allows for proper headlight mode

    drawGround();
    drawBoundaryWalls();
    drawSafeZoneIndicator();

    for (const auto& obj : gameObjects) {
        obj->draw();
    }

    // Draw targets
    for (const auto& target : targets) {
        target->draw();
    }

    // Draw bullets
    for (const auto& bullet : bullets) {
        bullet->draw();
    }
    
    for (const auto& shape : objects) {
        shape->draw();
    }
    
    // Draw enemies
    for (const auto& enemy : enemies) {
        enemy->draw();
    }

    drawGlassPanels();

}

void Scene::addGameObject(GameObject* obj) {
    gameObjects.push_back(obj);
}

void Scene::clearGameObjects() {
    for (auto& obj : gameObjects) {
        delete obj;
    }
    gameObjects.clear();
}

void Scene::addShape(std::shared_ptr<Shape> shape) {
    objects.push_back(shape);
}

void Scene::addGlassPanel(const Vector3& center, float width, float height, bool facingX, float normalSign) {
    glassPanels.push_back({center, width, height, facingX, normalSign});
}

void Scene::addTexture(Texture* texture) {
    textures.push_back(texture);
}

void Scene::addEnemy(Enemy* enemy) {
    enemies.push_back(enemy);
}

void Scene::rebuildCollisionGrid() {
    collisionGrid.initialize(objects);
}

void Scene::rebuildNavigationGrid() {
    navigationGrid.initialize(objects);
    navigationGrid.blockCircle(0.0f, 0.0f, SAFE_ZONE_RADIUS + NavigationGrid::ENEMY_RADIUS);
}

bool Scene::isInSafeZone(const Vector3& position, float margin) const {
    float dx = position.x;
    float dz = position.z;
    float effectiveRadius = SAFE_ZONE_RADIUS + margin;
    return dx * dx + dz * dz <= effectiveRadius * effectiveRadius;
}

void Scene::drawGround() const {
    glPushMatrix();
    glColor3f(groundColor.r, groundColor.g, groundColor.b);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);

    glVertex3f(-groundSize, 0.0f, -groundSize);
    glVertex3f(groundSize, 0.0f, -groundSize);
    glVertex3f(groundSize, 0.0f, groundSize);
    glVertex3f(-groundSize, 0.0f, groundSize);

    glEnd();
    glPopMatrix();
}

void Scene::drawBoundaryWalls() const {
    glPushMatrix();
    glColor3f(0.4f, 0.3f, 0.2f); // Brown/tan color for walls

    // North wall (positive Z)
    glPushMatrix();
    glTranslatef(0.0f, wallHeight / 2.0f, groundSize);
    glScalef(groundSize * 2.0f + wallThickness * 2.0f, wallHeight, wallThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // South wall (negative Z)
    glPushMatrix();
    glTranslatef(0.0f, wallHeight / 2.0f, -groundSize);
    glScalef(groundSize * 2.0f + wallThickness * 2.0f, wallHeight, wallThickness);
    glutSolidCube(1.0f);
    glPopMatrix();

    // East wall (positive X)
    glPushMatrix();
    glTranslatef(groundSize, wallHeight / 2.0f, 0.0f);
    glScalef(wallThickness, wallHeight, groundSize * 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // West wall (negative X)
    glPushMatrix();
    glTranslatef(-groundSize, wallHeight / 2.0f, 0.0f);
    glScalef(wallThickness, wallHeight, groundSize * 2.0f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

void Scene::drawSafeZoneIndicator() const {
    const float radius = SAFE_ZONE_RADIUS;
    const float height = SAFE_ZONE_LIGHT_HEIGHT;
    const int segments = SAFE_ZONE_CIRCLE_SEGMENTS;
    const float fillAlpha = playerInsideSafeZone ? 0.05f : 0.25f;
    const float outlineAlpha = 0.8f;

    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glColor4f(1.0f, 1.0f, 0.0f, fillAlpha);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        int idx = i % segments;
        float angle = (2.0f * M_PI * idx) / segments;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, 0.0f, z);
        glVertex3f(x, height, z);
    }
    glEnd();

    glLineWidth(2.0f);
    glColor4f(1.0f, 1.0f, 0.0f, outlineAlpha);

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * M_PI * i) / segments;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, 0.0f, z);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * M_PI * i) / segments;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, height, z);
    }
    glEnd();

    glBegin(GL_LINES);
    for (int i = 0; i < segments; i += 8) {
        float angle = (2.0f * M_PI * i) / segments;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glVertex3f(x, 0.0f, z);
        glVertex3f(x, height, z);
    }
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glPopAttrib();
}

void Scene::drawGlassPanels() const {
    if (glassPanels.empty()) {
        return;
    }

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glDisable(GL_TEXTURE_2D);

    GLfloat glassSpecular[] = {0.8f, 0.9f, 1.0f, 1.0f};
    GLfloat glassShininess[] = {96.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glassSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, glassShininess);

    for (const auto& panel : glassPanels) {
        float halfW = panel.width * 0.5f;
        float halfH = panel.height * 0.5f;
        glColor4f(0.6f, 0.8f, 1.0f, 0.25f);

        glBegin(GL_QUADS);
        if (panel.facingX) {
            glNormal3f(panel.normalSign, 0.0f, 0.0f);
            glVertex3f(panel.center.x, panel.center.y - halfH, panel.center.z - halfW);
            glVertex3f(panel.center.x, panel.center.y - halfH, panel.center.z + halfW);
            glVertex3f(panel.center.x, panel.center.y + halfH, panel.center.z + halfW);
            glVertex3f(panel.center.x, panel.center.y + halfH, panel.center.z - halfW);
        } else {
            glNormal3f(0.0f, 0.0f, panel.normalSign);
            glVertex3f(panel.center.x - halfW, panel.center.y - halfH, panel.center.z);
            glVertex3f(panel.center.x + halfW, panel.center.y - halfH, panel.center.z);
            glVertex3f(panel.center.x + halfW, panel.center.y + halfH, panel.center.z);
            glVertex3f(panel.center.x - halfW, panel.center.y + halfH, panel.center.z);
        }
        glEnd();
    }

    GLfloat defaultSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat defaultShininess[] = {32.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, defaultShininess);

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glPopAttrib();
}

// Get the highest ground level at a given XZ position (for landing on boxes)
float Scene::getGroundHeightAt(float x, float z, float radius, float currentY) const {
    float highestGround = 0.0f; // Start with ground level
    const float SKIN_WIDTH = 0.01f; // Small epsilon for stable landing

    // Check all shapes to find the highest one the player can land on
    for (const auto& shape : objects) {
        if (!shape) continue;

        Vector3 shapePos = shape->getPosition();
        Vector3 shapeSize = shape->getSize();

        // Calculate AABB of the shape
        float boxMinX = shapePos.x - shapeSize.x / 2.0f;
        float boxMaxX = shapePos.x + shapeSize.x / 2.0f;
        float boxMinZ = shapePos.z - shapeSize.z / 2.0f;
        float boxMaxZ = shapePos.z + shapeSize.z / 2.0f;
        float boxMaxY = shapePos.y + shapeSize.y / 2.0f;
        float boxMinY = shapePos.y - shapeSize.y / 2.0f;

        // Check if player's XZ position overlaps with this box (with radius)
        float closestX = (x < boxMinX) ? boxMinX : (x > boxMaxX) ? boxMaxX : x;
        float closestZ = (z < boxMinZ) ? boxMinZ : (z > boxMaxZ) ? boxMaxZ : z;

        float distX = x - closestX;
        float distZ = z - closestZ;
        float distanceSquared = distX * distX + distZ * distZ;

        // If player is above this box and overlaps in XZ
        if (distanceSquared <= (radius * radius) && currentY >= boxMinY) {
            // This box's top surface is a potential landing spot
            if (boxMaxY > highestGround && boxMaxY <= currentY + SKIN_WIDTH) {
                highestGround = boxMaxY;
            }
        }
    }

    return highestGround;
}

// Check vertical collision - returns true if landing on something, sets outGroundY
bool Scene::checkVerticalCollision(float x, float y, float z, float radius, float height, float& outGroundY) const {
    const float SKIN_WIDTH = 0.01f;
    float playerBottom = y;
    float playerTop = y + height;

    outGroundY = 0.0f; // Default ground level
    bool foundGround = false;

    // Check all shapes for vertical collision
    for (const auto& shape : objects) {
        if (!shape) continue;

        Vector3 shapePos = shape->getPosition();
        Vector3 shapeSize = shape->getSize();

        // Calculate AABB of the shape
        float boxMinX = shapePos.x - shapeSize.x / 2.0f;
        float boxMaxX = shapePos.x + shapeSize.x / 2.0f;
        float boxMinZ = shapePos.z - shapeSize.z / 2.0f;
        float boxMaxZ = shapePos.z + shapeSize.z / 2.0f;
        float boxMaxY = shapePos.y + shapeSize.y / 2.0f;
        float boxMinY = shapePos.y - shapeSize.y / 2.0f;

        // Check horizontal overlap first (player XZ vs box XZ)
        float closestX = (x < boxMinX) ? boxMinX : (x > boxMaxX) ? boxMaxX : x;
        float closestZ = (z < boxMinZ) ? boxMinZ : (z > boxMaxZ) ? boxMaxZ : z;

        float distX = x - closestX;
        float distZ = z - closestZ;
        float distanceSquared = distX * distX + distZ * distZ;

        // If player overlaps horizontally with this box
        if (distanceSquared <= (radius * radius)) {
            // Check if landing on top
            if (playerBottom <= boxMaxY + SKIN_WIDTH && playerBottom >= boxMinY) {
                if (boxMaxY > outGroundY) {
                    outGroundY = boxMaxY;
                    foundGround = true;
                }
            }
            // Check if hitting ceiling
            else if (playerTop >= boxMinY && playerTop <= boxMaxY + SKIN_WIDTH) {
                // Hitting bottom of box while jumping up - not handled here, just note overlap
            }
        }
    }

    return foundGround;
}

// Check collision with all objects and boundary walls (horizontal XZ collision only)
bool Scene::checkCollision(float x, float z, float radius) const {
    // Check boundary walls collision (simplified - treat as rectangle boundary)
    float boundaryMin = -groundSize + radius;
    float boundaryMax = groundSize - radius;

    if (x < boundaryMin || x > boundaryMax || z < boundaryMin || z > boundaryMax) {
        return true; // Collision with boundary walls
    }

    // Check collision with all game objects
    for (const auto& obj : gameObjects) {
        if (obj->checkAABBCollision(x, z, radius)) {
            return true; // Collision with object
        }
    }

    // NOTE: Shape collision checking removed from 2D version
    // Use the Y-aware checkCollision(x, y, z, radius, height) instead
    // This ensures proper collision detection when standing on boxes

    return false; // No collision
}

// Y-aware collision detection - checks if player at given height would collide with boxes
bool Scene::checkCollision(float x, float y, float z, float radius, float height) const {
    const float MARGIN = 0.1f; // Small margin to allow walking off edges

    // Check boundary walls collision
    float boundaryMin = -groundSize + radius;
    float boundaryMax = groundSize - radius;

    if (x < boundaryMin || x > boundaryMax || z < boundaryMin || z > boundaryMax) {
        return true; // Collision with boundary walls
    }

    // Check collision with all game objects (these use old 2D collision)
    for (const auto& obj : gameObjects) {
        if (obj->checkAABBCollision(x, z, radius)) {
            return true; // Collision with object
        }
    }

    // Check collision with shape objects - Y-aware
    for (const auto& shape : objects) {
        if (!shape) continue;

        Vector3 shapePos = shape->getPosition();
        Vector3 shapeSize = shape->getSize();

        // Calculate box bounds
        float boxMinX = shapePos.x - shapeSize.x / 2.0f;
        float boxMaxX = shapePos.x + shapeSize.x / 2.0f;
        float boxMinZ = shapePos.z - shapeSize.z / 2.0f;
        float boxMaxZ = shapePos.z + shapeSize.z / 2.0f;
        float boxMinY = shapePos.y - shapeSize.y / 2.0f;
        float boxMaxY = shapePos.y + shapeSize.y / 2.0f;

        // Player bounds
        float playerBottom = y;
        float playerTop = y + height;

        // CRITICAL: If player's feet are AT or ABOVE the box top, they're standing on it
        // Allow movement (no horizontal collision with this box)
        if (playerBottom >= boxMaxY - MARGIN) {
            continue; // Standing on top of box or above it, allow movement
        }

        // Player is below box top - check if they're actually inside the box vertically
        if (playerTop > boxMinY) {
            // Player vertically intersects with box, check XZ collision
            // Find closest point on box to player position in XZ plane
            float closestX = (x < boxMinX) ? boxMinX : (x > boxMaxX) ? boxMaxX : x;
            float closestZ = (z < boxMinZ) ? boxMinZ : (z > boxMaxZ) ? boxMaxZ : z;

            float distX = x - closestX;
            float distZ = z - closestZ;
            float distanceSquared = distX * distX + distZ * distZ;

            if (distanceSquared <= (radius * radius)) {
                return true; // Collision with shape sides
            }
        }
    }

    return false; // No collision
}

// Update scene (bullets, physics, etc.)
void Scene::update(float deltaTime) {
    for (auto& bullet : bullets) bullet->move(deltaTime);
    checkBulletCollisions();
    for (auto& bullet : bullets) {
        if(bullet->shouldRemove()) {
            delete bullet;
            bullets.erase(std::remove(bullets.begin(), bullets.end(), bullet), bullets.end());
        }
    }
}

void Scene::checkBulletCollisions() {
    for (auto& bullet : bullets) {
        if(!bullet->isActive()) continue;

        Vector3 bulletPos = bullet->getPosition();

        // Check wall collisions
        if (bulletPos.x < -groundSize || bulletPos.x > groundSize ||
            bulletPos.z < -groundSize || bulletPos.z > groundSize ||
            bulletPos.y < 0.0f || bulletPos.y > wallHeight) {
            bullet->deactivate();
            continue;
        }

        // Check Target collisions
        for (auto& target : targets) {
            if (!target->isAlive()) continue;
            if (target->checkAABBCollision(bulletPos.x, bulletPos.z, 0.1f)) {
                // Check Y coordinate (height) as well
                Vector3 targetPos = target->getPosition();
                Vector3 targetSize = target->getSize();
                if (bulletPos.y >= targetPos.y - targetSize.y &&
                    bulletPos.y <= targetPos.y + targetSize.y) {
                    bullet->deactivate();
                    target->takeDamage(bullet->getDamage());
                    break;
                }
            }
        }

        if(!bullet->isActive()) continue;

        // Check Enemy collisions
        for (auto& enemy : enemies) {
            if (CollisionDetector::checkCollision(bullet->getShape(), enemy->getHeadShape()) ||
                CollisionDetector::checkCollision(bullet->getShape(), enemy->getBodyShape())) {
                bullet->deactivate();
                enemy->takeDamage(bullet->getDamage());
                if(enemy->isAlive() == false) {
                    delete enemy;
                    enemies.erase(std::remove(enemies.begin(), enemies.end(), enemy), enemies.end());
                }
                break;
            }
        }

        if(!bullet->isActive()) continue;

        // Check Shape collisions (cubes, spheres, cylinders)
        for (auto& obj : objects) {
            if (CollisionDetector::checkCollision(bullet->getShape(), obj.get())) {
                bullet->deactivate();
                break;
            }
        }
    }
}

// Add a target to the scene
void Scene::addTarget(Target* target) {
    targets.push_back(target);
}

// Fire a bullet from a position in a direction
void Scene::fireBullet(const Vector3& position, const Vector3& direction) {
    bullets.push_back(new Bullet(position, direction));
}

// Clear all bullets
void Scene::clearBullets() {
    for (auto& bullet : bullets) {
        delete bullet;
    }
    bullets.clear();
}

// Calculate scene bounding sphere for camera zoom-to-fit
void Scene::calculateSceneBounds(Vector3& center, float& radius) const {
    // Initialize AABB to invalid range
    // Using minBounds/maxBounds to avoid Windows min/max macro conflicts
    Vector3 minBounds(1000000.0f, 1000000.0f, 1000000.0f);
    Vector3 maxBounds(-1000000.0f, -1000000.0f, -1000000.0f);

    bool foundAnyObject = false;

    // Include all Shape objects (cubes, spheres, cylinders, etc.)
    for (const auto& obj : objects) {
        Vector3 objPos = obj->getPosition();
        Vector3 objSize = obj->getSize();

        // Compute AABB of this object (assuming size is full extent, not half)
        Vector3 objMin(objPos.x - objSize.x / 2.0f, objPos.y - objSize.y / 2.0f, objPos.z - objSize.z / 2.0f);
        Vector3 objMax(objPos.x + objSize.x / 2.0f, objPos.y + objSize.y / 2.0f, objPos.z + objSize.z / 2.0f);

        // Expand scene AABB
        minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
        minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
        minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

        maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
        maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
        maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

        foundAnyObject = true;
    }

    // Include enemies (use body and head shapes for bounds)
    for (const auto& enemy : enemies) {
        // Get bounds from body shape
        Shape* bodyShape = enemy->getBodyShape();
        if (bodyShape) {
            Vector3 bodyPos = bodyShape->getPosition();
            Vector3 bodySize = bodyShape->getSize();

            Vector3 objMin(bodyPos.x - bodySize.x / 2.0f, bodyPos.y - bodySize.y / 2.0f, bodyPos.z - bodySize.z / 2.0f);
            Vector3 objMax(bodyPos.x + bodySize.x / 2.0f, bodyPos.y + bodySize.y / 2.0f, bodyPos.z + bodySize.z / 2.0f);

            minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
            minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
            minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

            maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
            maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
            maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

            foundAnyObject = true;
        }

        // Get bounds from head shape
        Shape* headShape = enemy->getHeadShape();
        if (headShape) {
            Vector3 headPos = headShape->getPosition();
            Vector3 headSize = headShape->getSize();

            Vector3 objMin(headPos.x - headSize.x / 2.0f, headPos.y - headSize.y / 2.0f, headPos.z - headSize.z / 2.0f);
            Vector3 objMax(headPos.x + headSize.x / 2.0f, headPos.y + headSize.y / 2.0f, headPos.z + headSize.z / 2.0f);

            minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
            minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
            minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

            maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
            maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
            maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

            foundAnyObject = true;
        }
    }

    // Include targets
    for (const auto& target : targets) {
        Vector3 targetPos = target->getPosition();
        Vector3 targetSize = target->getSize();

        Vector3 objMin(targetPos.x - targetSize.x / 2.0f, targetPos.y - targetSize.y / 2.0f, targetPos.z - targetSize.z / 2.0f);
        Vector3 objMax(targetPos.x + targetSize.x / 2.0f, targetPos.y + targetSize.y / 2.0f, targetPos.z + targetSize.z / 2.0f);

        minBounds.x = (minBounds.x < objMin.x) ? minBounds.x : objMin.x;
        minBounds.y = (minBounds.y < objMin.y) ? minBounds.y : objMin.y;
        minBounds.z = (minBounds.z < objMin.z) ? minBounds.z : objMin.z;

        maxBounds.x = (maxBounds.x > objMax.x) ? maxBounds.x : objMax.x;
        maxBounds.y = (maxBounds.y > objMax.y) ? maxBounds.y : objMax.y;
        maxBounds.z = (maxBounds.z > objMax.z) ? maxBounds.z : objMax.z;

        foundAnyObject = true;
    }

    // If no objects found, use default scene bounds (ground plane)
    if (!foundAnyObject) {
        minBounds = Vector3(-groundSize, 0.0f, -groundSize);
        maxBounds = Vector3(groundSize, wallHeight, groundSize);
    }

    // Calculate center of AABB
    center.x = (minBounds.x + maxBounds.x) / 2.0f;
    center.y = (minBounds.y + maxBounds.y) / 2.0f;
    center.z = (minBounds.z + maxBounds.z) / 2.0f;

    // Calculate radius as half the diagonal of AABB
    float dx = maxBounds.x - minBounds.x;
    float dy = maxBounds.y - minBounds.y;
    float dz = maxBounds.z - minBounds.z;
    radius = sqrt(dx * dx + dy * dy + dz * dz) / 2.0f;

    // Ensure minimum radius
    if (radius < 1.0f) {
        radius = 10.0f; // Default scene radius if very small
    }
}

void Scene::switchTexture() {
    static int textureIndex = 1;
    textureIndex = (textureIndex + 1) % textures.size();
    objects[8]->bindTexture(textures[textureIndex]);
}
