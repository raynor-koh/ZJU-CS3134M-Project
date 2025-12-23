#define _USE_MATH_DEFINES
#include "EnemyManager.h"
#include "Scene.h"
#include "Player.h"
#include "CollisionDetector.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>

EnemyManager::EnemyManager(Scene* scene)
    : scene(scene),
      spawnTimer(0.0f),
      spawnInterval(3.0f),        // Spawn every 3 seconds
      minSpawnRadius(8.0f),       // At least 8 units from player
      maxSpawnRadius(25.0f),      // At most 25 units from player
      maxEnemies(10),             // Maximum 10 enemies at once
      enemySpeed(3.0f),           // Enemy move speed
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
    // Pick random angle
    float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * static_cast<float>(M_PI);

    // Pick random distance between min and max spawn radius
    float distance = minSpawnRadius + (static_cast<float>(rand()) / RAND_MAX) * (maxSpawnRadius - minSpawnRadius);

    // Calculate spawn position
    float spawnX = playerPos.x + cos(angle) * distance;
    float spawnZ = playerPos.z + sin(angle) * distance;
    Vector3 spawnPos(spawnX, 0.0f, spawnZ);

    // Validate spawn position
    if (!isValidSpawnPosition(spawnPos)) {
        // Try a few more times with different angles
        for (int attempt = 0; attempt < 5; attempt++) {
            angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * static_cast<float>(M_PI);
            spawnX = playerPos.x + cos(angle) * distance;
            spawnZ = playerPos.z + sin(angle) * distance;
            spawnPos = Vector3(spawnX, 0.0f, spawnZ);
            if (isValidSpawnPosition(spawnPos)) {
                break;
            }
        }
    }

    // Create enemy with random colors
    float r1 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float g1 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float b1 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float r2 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float g2 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;
    float b2 = 0.3f + (static_cast<float>(rand()) / RAND_MAX) * 0.7f;

    Enemy* enemy = new Enemy(spawnPos, Color(r1, g1, b1), Color(r2, g2, b2));
    managedEnemies.push_back(enemy);
    scene->addEnemy(enemy);

    std::cout << "Enemy spawned at (" << spawnX << ", 0, " << spawnZ
              << ") - distance from player: " << distance
              << " - total enemies: " << managedEnemies.size() << std::endl;
}

void EnemyManager::updateEnemyMovement(float deltaTime, const Vector3& playerPos) {
    for (Enemy* enemy : managedEnemies) {
        if (!enemy || !enemy->isAlive()) continue;

        Vector3 enemyPos = enemy->getPosition();

        // Calculate direction to player (in XZ plane)
        float dx = playerPos.x - enemyPos.x;
        float dz = playerPos.z - enemyPos.z;
        float distance = sqrt(dx * dx + dz * dz);

        if (distance > 0.1f) {  // Only move if not already at player
            // Normalize direction
            dx /= distance;
            dz /= distance;

            // Move toward player
            float moveX = dx * enemySpeed * deltaTime;
            float moveZ = dz * enemySpeed * deltaTime;

            Vector3 newPos(enemyPos.x + moveX, enemyPos.y, enemyPos.z + moveZ);
            enemy->setPosition(newPos);
        }
    }
}

float EnemyManager::checkPlayerCollision(Player* player, float deltaTime) {
    if (!player || damageCooldown > 0.0f) {
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

void EnemyManager::removeDeadEnemies() {
    // Note: Scene also removes dead enemies in checkBulletCollisions
    // We just need to clean up our tracking list
    auto it = managedEnemies.begin();
    while (it != managedEnemies.end()) {
        if (*it == nullptr || !(*it)->isAlive()) {
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

    // Check collision with scene objects using a simple radius check
    const float enemyRadius = 1.0f;
    return !scene->checkCollision(pos.x, pos.z, enemyRadius);
}

void EnemyManager::clear() {
    // Note: Enemies are owned by Scene, so we don't delete them here
    managedEnemies.clear();
    spawnTimer = 0.0f;
    damageCooldown = 0.0f;
}

void EnemyManager::reset() {
    clear();
    spawnTimer = 0.0f;
    damageCooldown = 0.0f;
}
