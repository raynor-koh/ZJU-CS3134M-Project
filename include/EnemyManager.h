#pragma once

#include <vector>
#include <memory>
#include "Enemy.h"
#include "GameState.h"

class Scene;  // Forward declaration
class Player; // Forward declaration
class Shape;  // Forward declaration

class EnemyManager {
public:
    EnemyManager(Scene* scene);
    ~EnemyManager();

    // Core update - only runs when state is PLAYING
    void update(float deltaTime, GameState state, const Vector3& playerPos);

    // Spawning
    void setSpawnInterval(float interval) { spawnInterval = interval; }
    void setMinSpawnRadius(float radius) { minSpawnRadius = radius; }
    void setMaxSpawnRadius(float radius) { maxSpawnRadius = radius; }
    void setMaxEnemies(int count) { maxEnemies = count; }

    // Enemy properties
    void setEnemySpeed(float speed) { enemySpeed = speed; }

    // Obstacle avoidance tuning
    void setAvoidanceLookAhead(float distance) { avoidanceLookAhead = distance; }
    void setAvoidanceStrength(float strength) { avoidanceStrength = strength; }
    void setAvoidanceRayCount(int count) { avoidanceRayCount = count; }
    void setAvoidanceRaySpread(float angleDeg) { avoidanceRaySpread = angleDeg; }

    // Check collision with player, returns damage dealt
    float checkPlayerCollision(Player* player, float deltaTime);

    // Get enemy count for debugging
    int getEnemyCount() const { return static_cast<int>(managedEnemies.size()); }

    // Clear all managed enemies
    void clear();

    // Reset manager state (for new game)
    void reset();

private:
    void spawnEnemy(const Vector3& playerPos);
    void updateEnemyMovement(float deltaTime, const Vector3& playerPos);
    void removeDeadEnemies();

    // Check if spawn position is valid (not inside geometry)
    bool isValidSpawnPosition(const Vector3& pos) const;

    // Obstacle avoidance helpers
    Vector3 computeAvoidanceForce(const Vector3& enemyPos, const Vector3& desiredDir, float enemyRadius) const;
    bool rayIntersectsAABB(const Vector3& rayOrigin, const Vector3& rayDir,
                           const Vector3& boxMin, const Vector3& boxMax,
                           float& hitDistance, Vector3& hitNormal) const;

    Scene* scene;
    std::vector<Enemy*> managedEnemies;  // Enemies this manager spawned and controls

    // Spawn parameters
    float spawnTimer;
    float spawnInterval;      // Seconds between spawns
    float minSpawnRadius;     // Minimum distance from player
    float maxSpawnRadius;     // Maximum distance from player
    int maxEnemies;           // Maximum enemies alive at once

    // Enemy properties
    float enemySpeed;

    // Obstacle avoidance parameters (tunable)
    float avoidanceLookAhead;   // How far ahead to check for obstacles (default: 3.0)
    float avoidanceStrength;    // How strongly to avoid obstacles (default: 2.0)
    int avoidanceRayCount;      // Number of rays to cast (default: 5 - center + 2 on each side)
    float avoidanceRaySpread;   // Angle spread for side rays in degrees (default: 45.0)

    // Player damage tracking
    float damageCooldown;     // Time until player can be damaged again
    float damagePerHit;       // Damage dealt per hit
    float hitCooldownDuration;// Duration of invincibility after hit
};
