#include "CollisionGrid.h"
#include <cmath>
#include <iostream>

CollisionGrid::CollisionGrid() {
    // Initialize all cells to empty
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int z = 0; z < GRID_SIZE; z++) {
            grid[x][z].occupied = false;
            grid[x][z].maxObstacleHeight = 0.0f;
        }
    }
}

void CollisionGrid::initialize(const std::vector<std::shared_ptr<Shape>>& obstacles) {
    // Clear all cells first
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int z = 0; z < GRID_SIZE; z++) {
            grid[x][z].occupied = false;
            grid[x][z].maxObstacleHeight = 0.0f;
        }
    }

    obstacleBounds.clear();
    int totalCellsMarked = 0;

    // For each obstacle, compute AABB and mark all overlapping cells
    for (const auto& shape : obstacles) {
        if (!shape) continue;

        Vector3 pos = shape->getPosition();
        Vector3 size = shape->getSize();

        // Compute world-space AABB from shape position + size
        // Shape position is CENTER, size is FULL extent
        float minX = pos.x - size.x / 2.0f;
        float maxX = pos.x + size.x / 2.0f;
        float minZ = pos.z - size.z / 2.0f;
        float maxZ = pos.z + size.z / 2.0f;
        float minY = pos.y - size.y / 2.0f;
        float maxY = pos.y + size.y / 2.0f;  // Top of obstacle

        obstacleBounds.push_back({minX, maxX, minZ, maxZ, minY, maxY});

        // ADD enemy radius padding HERE (only place - not in canMoveTo)
        minX -= ENEMY_RADIUS;
        maxX += ENEMY_RADIUS;
        minZ -= ENEMY_RADIUS;
        maxZ += ENEMY_RADIUS;

        // Convert to grid indices and MARK ALL INTERSECTING CELLS
        int gridMinX = clampInt(worldToGridX(minX), 0, GRID_SIZE - 1);
        int gridMaxX = clampInt(worldToGridX(maxX), 0, GRID_SIZE - 1);
        int gridMinZ = clampInt(worldToGridZ(minZ), 0, GRID_SIZE - 1);
        int gridMaxZ = clampInt(worldToGridZ(maxZ), 0, GRID_SIZE - 1);

        // Mark all cells that the padded AABB intersects
        for (int gx = gridMinX; gx <= gridMaxX; gx++) {
            for (int gz = gridMinZ; gz <= gridMaxZ; gz++) {
                markCell(gx, gz, maxY);
                totalCellsMarked++;
            }
        }
    }

    std::cout << "CollisionGrid initialized: " << obstacles.size() << " obstacles, "
              << totalCellsMarked << " cells marked" << std::endl;
}

bool CollisionGrid::canMoveTo(float x, float z, float entityHeight) const {
    // Out-of-bounds = BLOCKED (boundary walls)
    const float BOUNDARY = 48.0f;  // 50 - enemy radius buffer
    if (x < -BOUNDARY || x > BOUNDARY || z < -BOUNDARY || z > BOUNDARY) {
        return false;
    }

    // Convert position to grid cell (enemy center point)
    int gx = worldToGridX(x);
    int gz = worldToGridZ(z);

    // Clamp to valid range (safety check after boundary check)
    if (gx < 0 || gx >= GRID_SIZE || gz < 0 || gz >= GRID_SIZE) {
        return false;
    }

    // Check if cell is occupied
    if (!grid[gx][gz].occupied) {
        return true;
    }

    float entityBottom = 0.0f;
    float entityTop = entityHeight;

    for (const auto& bounds : obstacleBounds) {
        if (entityTop <= bounds.minY || entityBottom >= bounds.maxY) {
            continue;
        }

        float closestX = (std::max)(bounds.minX, (std::min)(x, bounds.maxX));
        float closestZ = (std::max)(bounds.minZ, (std::min)(z, bounds.maxZ));
        float dx = x - closestX;
        float dz = z - closestZ;

        if ((dx * dx + dz * dz) <= (ENEMY_RADIUS * ENEMY_RADIUS)) {
            return false;
        }
    }

    return true;
}

bool CollisionGrid::isOccupied(int gridX, int gridZ) const {
    if (gridX < 0 || gridX >= GRID_SIZE || gridZ < 0 || gridZ >= GRID_SIZE) {
        return true;  // Out of bounds treated as occupied
    }
    return grid[gridX][gridZ].occupied;
}

float CollisionGrid::getMaxHeight(int gridX, int gridZ) const {
    if (gridX < 0 || gridX >= GRID_SIZE || gridZ < 0 || gridZ >= GRID_SIZE) {
        return 0.0f;
    }
    return grid[gridX][gridZ].maxObstacleHeight;
}

int CollisionGrid::worldToGridX(float x) const {
    return static_cast<int>((x + GRID_OFFSET) / CELL_SIZE);
}

int CollisionGrid::worldToGridZ(float z) const {
    return static_cast<int>((z + GRID_OFFSET) / CELL_SIZE);
}

float CollisionGrid::gridToWorldX(int gx) const {
    return gx * CELL_SIZE - GRID_OFFSET + CELL_SIZE / 2.0f;
}

float CollisionGrid::gridToWorldZ(int gz) const {
    return gz * CELL_SIZE - GRID_OFFSET + CELL_SIZE / 2.0f;
}

void CollisionGrid::markCell(int gx, int gz, float maxY) {
    if (gx < 0 || gx >= GRID_SIZE || gz < 0 || gz >= GRID_SIZE) return;

    grid[gx][gz].occupied = true;
    if (maxY > grid[gx][gz].maxObstacleHeight) {
        grid[gx][gz].maxObstacleHeight = maxY;
    }
}

int CollisionGrid::clampInt(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}
