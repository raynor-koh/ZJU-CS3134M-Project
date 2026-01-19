#define _USE_MATH_DEFINES
#include "EnemyManager.h"
#include "Scene.h"
#include "Player.h"
#include "NavigationGrid.h"
#include "CollisionDetector.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

namespace {
    float randomRange(float minVal, float maxVal) {
        return minVal + (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * (maxVal - minVal);
    }

    Vector3 sampleWanderTarget(const Scene* scene) {
        float worldLimit = 45.0f;
        if (scene) {
            worldLimit = scene->getGroundSize();
        }
        worldLimit = (std::max)(worldLimit - 2.0f, 5.0f);

        Vector3 candidate;
        candidate.y = 0.0f;
        int attempts = 0;
        do {
            candidate.x = randomRange(-worldLimit, worldLimit);
            candidate.z = randomRange(-worldLimit, worldLimit);
            attempts++;
            if (attempts >= 10) break;
        } while (scene && scene->isInSafeZone(Vector3(candidate.x, 0.0f, candidate.z)));

        return candidate;
    }
}

EnemyManager::EnemyManager(Scene* scene)
    : scene(scene),
      spawnTimer(0.0f),
      spawnInterval(3.0f),        // Spawn every 3 seconds
      minSpawnRadius(8.0f),       // At least 8 units from player
      maxSpawnRadius(25.0f),      // At most 25 units from player
      maxEnemies(10),             // Maximum 10 enemies at once
      enemySpeed(3.0f),           // Enemy move speed
      navigationGrid(nullptr),
      damageCooldown(0.0f),
      damagePerHit(10.0f),        // 10 damage per hit
      hitCooldownDuration(1.0f)   // 1 second invincibility
{
    // Seed random number generator
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }
}

EnemyManager::~EnemyManager() {
    clear();
}

void EnemyManager::update(float deltaTime, GameState state, const Vector3& playerPos) {
    // Only update if playing
    if (state != GameState::PLAYING) {
        return;
    }

    // Update damage cooldown
    if (damageCooldown > 0.0f) {
        damageCooldown -= deltaTime;
    }

    // Update spawn timer
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval && static_cast<int>(managedEnemies.size()) < maxEnemies) {
        spawnEnemy(playerPos);
        spawnTimer = 0.0f;
    }

    // Update enemy movement
    updateEnemyMovement(deltaTime, playerPos);

    // Remove dead enemies
    removeDeadEnemies();
}

void EnemyManager::spawnEnemy(const Vector3& playerPos) {
    const int maxAttempts = 12;
    Vector3 spawnPos(0.0f, 0.0f, 0.0f);
    bool validSpawn = false;

    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * static_cast<float>(M_PI);
        float distance = minSpawnRadius + (static_cast<float>(rand()) / RAND_MAX) * (maxSpawnRadius - minSpawnRadius);
        float spawnX = playerPos.x + cos(angle) * distance;
        float spawnZ = playerPos.z + sin(angle) * distance;
        spawnPos = Vector3(spawnX, 0.0f, spawnZ);

        if (!isValidSpawnPosition(spawnPos)) {
            continue;
        }
        if (!hasPathToPlayer(spawnPos, playerPos)) {
            continue;
        }

        validSpawn = true;
        break;
    }

    if (!validSpawn) {
        return;
    }

    float spawnX = spawnPos.x;
    float spawnZ = spawnPos.z;
    float distance = std::sqrt((spawnX - playerPos.x) * (spawnX - playerPos.x) +
                               (spawnZ - playerPos.z) * (spawnZ - playerPos.z));

    // Create enemy with random colors
    float r1 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float g1 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float b1 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float r2 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float g2 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float b2 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;

    Enemy* enemy = new Enemy(spawnPos, Color(r1, g1, b1), Color(r2, g2, b2));

    // Calculate initial yaw to face player, then rotate 90 degrees right
    float dx = playerPos.x - spawnX;
    float dz = playerPos.z - spawnZ;
    float initialYaw = atan2(dz, dx) - static_cast<float>(M_PI) / 2.0f;
    enemy->setYaw(initialYaw);

    managedEnemies.push_back(enemy);
    scene->addEnemy(enemy);

    std::cout << "Enemy spawned at (" << spawnX << ", 0, " << spawnZ
              << ") - distance from player: " << distance
              << " - total enemies: " << managedEnemies.size() << std::endl;
}

void EnemyManager::updateEnemyMovement(float deltaTime, const Vector3& playerPos) {
    const float WAYPOINT_THRESHOLD = 0.5f;
    const float REPLAN_INTERVAL = 0.5f;
    const float SAFE_ZONE_REPLAN_INTERVAL = 1.5f;
    const float MOVE_EPSILON = 0.01f;
    const bool playerInSafeZone = scene && scene->isInSafeZone(playerPos);

    for (Enemy* enemy : managedEnemies) {
        if (!enemy || !enemy->isAlive()) continue;

        Vector3 enemyPos = enemy->getPosition();
        Vector3 targetPos = playerPos;

        if (navigationGrid) {
            EnemyPathInfo& info = pathInfo[enemy];
            info.replanTimer -= deltaTime;

            if (playerInSafeZone) {
                info.wandering = true;
                if (info.path.empty() || info.replanTimer <= 0.0f || info.nextIndex >= info.path.size()) {
                    info.path.clear();
                    Vector3 wanderTarget = sampleWanderTarget(scene);
                    navigationGrid->findPath(enemyPos.x, enemyPos.z, wanderTarget.x, wanderTarget.z, info.path);
                    info.nextIndex = 0;
                    info.replanTimer = SAFE_ZONE_REPLAN_INTERVAL;
                }
            } else {
                if (info.wandering) {
                    info.wandering = false;
                    info.nextIndex = 0;
                    info.path.clear();
                }

                int startGX = navigationGrid->worldToGridX(enemyPos.x);
                int startGZ = navigationGrid->worldToGridZ(enemyPos.z);
                int goalGX = navigationGrid->worldToGridX(playerPos.x);
                int goalGZ = navigationGrid->worldToGridZ(playerPos.z);

                bool needReplan = info.path.empty() ||
                                  info.replanTimer <= 0.0f ||
                                  startGX != info.lastStartX || startGZ != info.lastStartZ ||
                                  goalGX != info.lastGoalX || goalGZ != info.lastGoalZ ||
                                  info.nextIndex >= info.path.size();

                if (needReplan) {
                    info.path.clear();
                    navigationGrid->findPath(enemyPos.x, enemyPos.z, playerPos.x, playerPos.z, info.path);
                    info.nextIndex = 0;
                    info.replanTimer = REPLAN_INTERVAL;
                    info.lastStartX = startGX;
                    info.lastStartZ = startGZ;
                    info.lastGoalX = goalGX;
                    info.lastGoalZ = goalGZ;
                }
            }

            if (info.nextIndex < info.path.size()) {
                targetPos = info.path[info.nextIndex];
                float dxw = targetPos.x - enemyPos.x;
                float dzw = targetPos.z - enemyPos.z;
                float distw = std::sqrt(dxw * dxw + dzw * dzw);
                if (distw < WAYPOINT_THRESHOLD) {
                    info.nextIndex++;
                    if (info.nextIndex < info.path.size()) {
                        targetPos = info.path[info.nextIndex];
                    } else if (playerInSafeZone) {
                        targetPos = enemyPos;
                    } else {
                        targetPos = playerPos;
                    }
                }
            }
        }

        float dx = targetPos.x - enemyPos.x;
        float dz = targetPos.z - enemyPos.z;
        float distance = sqrt(dx * dx + dz * dz);

        if (distance > 0.01f) {
            float angleToTarget = atan2(dz, dx) - static_cast<float>(M_PI) / 2.0f;
            enemy->setYaw(angleToTarget);
        }

        if (distance > 0.1f) {
            dx /= distance;
            dz /= distance;

            float step = enemySpeed * deltaTime;
            float moveX = dx * step;
            float moveZ = dz * step;

            float newX = enemyPos.x + moveX;
            float newZ = enemyPos.z + moveZ;

            if (canMoveTo(newX, newZ)) {
                enemy->setPosition(Vector3(newX, enemyPos.y, newZ));
            } else if (std::fabs(moveX) > MOVE_EPSILON && canMoveTo(newX, enemyPos.z)) {
                enemy->setPosition(Vector3(newX, enemyPos.y, enemyPos.z));
            } else if (std::fabs(moveZ) > MOVE_EPSILON && canMoveTo(enemyPos.x, newZ)) {
                enemy->setPosition(Vector3(enemyPos.x, enemyPos.y, newZ));
            }
        }
    }
}

float EnemyManager::checkPlayerCollision(Player* player, float deltaTime) {
    if (!player || damageCooldown > 0.0f) {
        return 0.0f;
    }

    if (scene && scene->isInSafeZone(player->getPosition())) {
        return 0.0f;
    }

    // Use Player's existing collision check with enemies
    if (player->checkCollision(managedEnemies)) {
        damageCooldown = hitCooldownDuration;
        std::cout << "Player hit by enemy! Damage: " << damagePerHit << std::endl;
        return damagePerHit;
    }

    return 0.0f;
}

bool EnemyManager::hasPathToPlayer(const Vector3& spawnPos, const Vector3& playerPos) const {
    if (!navigationGrid || !scene) {
        return true;
    }

    Vector3 targetPos = playerPos;
    if (scene->isInSafeZone(playerPos)) {
        targetPos = sampleWanderTarget(scene);
    }

    std::vector<Vector3> path;
    if (!navigationGrid->findPath(spawnPos.x, spawnPos.z, targetPos.x, targetPos.z, path)) {
        return false;
    }
    return !path.empty();
}

void EnemyManager::removeDeadEnemies() {
    // Note: Scene also removes dead enemies in checkBulletCollisions
    // We just need to clean up our tracking list
    auto it = managedEnemies.begin();
    while (it != managedEnemies.end()) {
        if (*it == nullptr || !(*it)->isAlive()) {
            pathInfo.erase(*it);
            it = managedEnemies.erase(it);
        } else {
            ++it;
        }
    }
}

bool EnemyManager::isValidSpawnPosition(const Vector3& pos) const {
    if (!scene) return true;

    // Check boundaries (ground size is typically 50)
    const float groundSize = 45.0f;  // Slightly smaller than actual ground
    if (pos.x < -groundSize || pos.x > groundSize ||
        pos.z < -groundSize || pos.z > groundSize) {
        return false;
    }

    const float enemyRadius = 1.8f;
    if (scene->isInSafeZone(pos, enemyRadius)) {
        return false;
    }

    return !scene->checkCollision(pos.x, 0.0f, pos.z, enemyRadius, ENEMY_COLLISION_HEIGHT);
}

void EnemyManager::clear() {
    // Note: Enemies are owned by Scene, so we don't delete them here
    managedEnemies.clear();
    pathInfo.clear();
    spawnTimer = 0.0f;
    damageCooldown = 0.0f;
}

void EnemyManager::reset() {
    clear();
    spawnTimer = 0.0f;
    damageCooldown = 0.0f;
}

bool EnemyManager::canMoveTo(float x, float z) const {
    if (!scene) return true;
    const float ENEMY_RADIUS = 1.8f;
    const float ENEMY_HEIGHT = ENEMY_COLLISION_HEIGHT;
    if (scene->isInSafeZone(Vector3(x, 0.0f, z), ENEMY_RADIUS)) {
        return false;
    }
    return !scene->checkCollision(x, 0.0f, z, ENEMY_RADIUS, ENEMY_HEIGHT);
}
