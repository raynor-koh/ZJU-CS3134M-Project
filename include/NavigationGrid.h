#pragma once

#include "Shapes.h"
#include <vector>
#include <memory>

class NavigationGrid {
public:
    static constexpr float CELL_SIZE = 1.0f;
    static constexpr int GRID_SIZE = 100;       // 100 / 1 = 100 cells per axis
    static constexpr float GRID_OFFSET = 50.0f; // Offset to convert world coords to grid indices
    static constexpr float ENEMY_RADIUS = 1.8f; // Radius used when marking blocked cells

    NavigationGrid();

    void initialize(const std::vector<std::shared_ptr<Shape>>& obstacles);

    bool findPath(float startX, float startZ, float goalX, float goalZ,
                  std::vector<Vector3>& outPath) const;

    bool isBlocked(int gx, int gz) const;
    int worldToGridX(float x) const;
    int worldToGridZ(float z) const;
    float gridToWorldX(int gx) const;
    float gridToWorldZ(int gz) const;

private:
    struct Cell {
        bool blocked;
    };

    Cell grid[GRID_SIZE][GRID_SIZE];

    bool isInBounds(int gx, int gz) const;
    bool findNearestFreeCell(int startX, int startZ, int& outX, int& outZ) const;
    static int clampInt(int value, int minVal, int maxVal);
};
