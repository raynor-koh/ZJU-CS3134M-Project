#pragma once

#include "Shapes.h"
#include <vector>
#include <memory>
#include <algorithm>

struct GridCell {
    bool occupied;           // Is there any obstacle in this cell?
    float maxObstacleHeight; // Highest Y extent of obstacles in this cell (min is always 0)
};

class CollisionGrid {
public:
    static constexpr float CELL_SIZE = 2.0f;
    static constexpr int GRID_SIZE = 50;       // 100 / 2 = 50 cells per axis
    static constexpr float GRID_OFFSET = 50.0f; // Offset to convert world coords to grid indices
    static constexpr float ENEMY_RADIUS = 1.8f; // Padding added during marking (only place)

    CollisionGrid();

    // Initialize the grid from obstacle shapes
    void initialize(const std::vector<std::shared_ptr<Shape>>& obstacles);

    // Check if an entity can move to the given position
    // entityHeight: total height of the entity (for vertical collision check)
    bool canMoveTo(float x, float z, float entityHeight) const;

    // Check if a specific cell is occupied
    bool isOccupied(int gridX, int gridZ) const;

    // Get max obstacle height at a cell
    float getMaxHeight(int gridX, int gridZ) const;

    // Convert world coordinates to grid indices
    int worldToGridX(float x) const;
    int worldToGridZ(float z) const;

    // Convert grid indices to world coordinates (cell center)
    float gridToWorldX(int gx) const;
    float gridToWorldZ(int gz) const;

private:
    struct ObstacleBounds {
        float minX;
        float maxX;
        float minZ;
        float maxZ;
        float minY;
        float maxY;
    };

    GridCell grid[GRID_SIZE][GRID_SIZE];
    std::vector<ObstacleBounds> obstacleBounds;

    // Helper to mark a cell as occupied
    void markCell(int gx, int gz, float maxY);

    // Helper to clamp value to range
    static int clampInt(int value, int minVal, int maxVal);
};
