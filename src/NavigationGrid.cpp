#include "NavigationGrid.h"
#include <queue>
#include <utility>
#include <cmath>
#include <limits>

NavigationGrid::NavigationGrid() {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int z = 0; z < GRID_SIZE; z++) {
            grid[x][z].blocked = false;
        }
    }
}

void NavigationGrid::initialize(const std::vector<std::shared_ptr<Shape>>& obstacles) {
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int z = 0; z < GRID_SIZE; z++) {
            grid[x][z].blocked = false;
        }
    }

    for (const auto& shape : obstacles) {
        if (!shape) continue;

        Vector3 pos = shape->getPosition();
        Vector3 size = shape->getSize();

        float minX = pos.x - size.x / 2.0f - ENEMY_RADIUS;
        float maxX = pos.x + size.x / 2.0f + ENEMY_RADIUS;
        float minZ = pos.z - size.z / 2.0f - ENEMY_RADIUS;
        float maxZ = pos.z + size.z / 2.0f + ENEMY_RADIUS;

        int gridMinX = clampInt(worldToGridX(minX), 0, GRID_SIZE - 1);
        int gridMaxX = clampInt(worldToGridX(maxX), 0, GRID_SIZE - 1);
        int gridMinZ = clampInt(worldToGridZ(minZ), 0, GRID_SIZE - 1);
        int gridMaxZ = clampInt(worldToGridZ(maxZ), 0, GRID_SIZE - 1);

        for (int gx = gridMinX; gx <= gridMaxX; gx++) {
            for (int gz = gridMinZ; gz <= gridMaxZ; gz++) {
                grid[gx][gz].blocked = true;
            }
        }
    }
}

bool NavigationGrid::findPath(float startX, float startZ, float goalX, float goalZ,
                              std::vector<Vector3>& outPath) const {
    int startGX = clampInt(worldToGridX(startX), 0, GRID_SIZE - 1);
    int startGZ = clampInt(worldToGridZ(startZ), 0, GRID_SIZE - 1);
    int goalGX = clampInt(worldToGridX(goalX), 0, GRID_SIZE - 1);
    int goalGZ = clampInt(worldToGridZ(goalZ), 0, GRID_SIZE - 1);

    if (!findNearestFreeCell(startGX, startGZ, startGX, startGZ)) {
        return false;
    }
    if (!findNearestFreeCell(goalGX, goalGZ, goalGX, goalGZ)) {
        return false;
    }

    const int total = GRID_SIZE * GRID_SIZE;
    const float INF = std::numeric_limits<float>::infinity();
    std::vector<float> gScore(total, INF);
    std::vector<int> parent(total, -1);
    std::vector<bool> closed(total, false);

    auto idx = [](int gx, int gz) { return gz * GRID_SIZE + gx; };

    struct OpenNode {
        float f;
        int index;
    };
    struct OpenCompare {
        bool operator()(const OpenNode& a, const OpenNode& b) const {
            return a.f > b.f;
        }
    };

    auto heuristic = [](int ax, int az, int bx, int bz) {
        float dx = static_cast<float>(ax - bx);
        float dz = static_cast<float>(az - bz);
        return std::sqrt(dx * dx + dz * dz);
    };

    int startIdx = idx(startGX, startGZ);
    int goalIdx = idx(goalGX, goalGZ);

    gScore[startIdx] = 0.0f;
    std::priority_queue<OpenNode, std::vector<OpenNode>, OpenCompare> open;
    open.push({heuristic(startGX, startGZ, goalGX, goalGZ), startIdx});

    const int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    bool found = false;
    while (!open.empty()) {
        OpenNode current = open.top();
        open.pop();

        int curIdx = current.index;
        if (closed[curIdx]) continue;
        closed[curIdx] = true;

        if (curIdx == goalIdx) {
            found = true;
            break;
        }

        int curGX = curIdx % GRID_SIZE;
        int curGZ = curIdx / GRID_SIZE;

        for (const auto& dir : directions) {
            int nextGX = curGX + dir[0];
            int nextGZ = curGZ + dir[1];

            if (!isInBounds(nextGX, nextGZ) || isBlocked(nextGX, nextGZ)) {
                continue;
            }

            bool diagonal = (dir[0] != 0 && dir[1] != 0);
            if (diagonal) {
                if (isBlocked(curGX + dir[0], curGZ) || isBlocked(curGX, curGZ + dir[1])) {
                    continue;
                }
            }

            int nextIdx = idx(nextGX, nextGZ);
            float stepCost = diagonal ? 1.41421356f : 1.0f;
            float tentative = gScore[curIdx] + stepCost;

            if (tentative < gScore[nextIdx]) {
                gScore[nextIdx] = tentative;
                parent[nextIdx] = curIdx;
                float f = tentative + heuristic(nextGX, nextGZ, goalGX, goalGZ);
                open.push({f, nextIdx});
            }
        }
    }

    outPath.clear();
    if (!found) {
        return false;
    }

    std::vector<int> reverse;
    int cur = goalIdx;
    while (cur != -1) {
        reverse.push_back(cur);
        cur = parent[cur];
    }

    for (int i = static_cast<int>(reverse.size()) - 2; i >= 0; i--) {
        int cellIdx = reverse[i];
        int gx = cellIdx % GRID_SIZE;
        int gz = cellIdx / GRID_SIZE;
        outPath.push_back(Vector3(gridToWorldX(gx), 0.0f, gridToWorldZ(gz)));
    }

    return true;
}

bool NavigationGrid::isBlocked(int gx, int gz) const {
    if (!isInBounds(gx, gz)) {
        return true;
    }
    return grid[gx][gz].blocked;
}

int NavigationGrid::worldToGridX(float x) const {
    return static_cast<int>((x + GRID_OFFSET) / CELL_SIZE);
}

int NavigationGrid::worldToGridZ(float z) const {
    return static_cast<int>((z + GRID_OFFSET) / CELL_SIZE);
}

float NavigationGrid::gridToWorldX(int gx) const {
    return gx * CELL_SIZE - GRID_OFFSET + CELL_SIZE / 2.0f;
}

float NavigationGrid::gridToWorldZ(int gz) const {
    return gz * CELL_SIZE - GRID_OFFSET + CELL_SIZE / 2.0f;
}

bool NavigationGrid::isInBounds(int gx, int gz) const {
    return gx >= 0 && gx < GRID_SIZE && gz >= 0 && gz < GRID_SIZE;
}

bool NavigationGrid::findNearestFreeCell(int startX, int startZ, int& outX, int& outZ) const {
    if (!isInBounds(startX, startZ)) {
        startX = clampInt(startX, 0, GRID_SIZE - 1);
        startZ = clampInt(startZ, 0, GRID_SIZE - 1);
    }

    if (!isBlocked(startX, startZ)) {
        outX = startX;
        outZ = startZ;
        return true;
    }

    std::queue<std::pair<int, int>> queue;
    std::vector<bool> visited(GRID_SIZE * GRID_SIZE, false);
    auto idx = [](int gx, int gz) { return gz * GRID_SIZE + gx; };

    queue.push({startX, startZ});
    visited[idx(startX, startZ)] = true;

    const int directions[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        for (const auto& dir : directions) {
            int nx = current.first + dir[0];
            int nz = current.second + dir[1];

            if (!isInBounds(nx, nz)) {
                continue;
            }
            int nidx = idx(nx, nz);
            if (visited[nidx]) {
                continue;
            }

            if (!isBlocked(nx, nz)) {
                outX = nx;
                outZ = nz;
                return true;
            }

            visited[nidx] = true;
            queue.push({nx, nz});
        }
    }

    return false;
}

int NavigationGrid::clampInt(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

void NavigationGrid::blockCircle(float centerX, float centerZ, float radius) {
    float minX = centerX - radius;
    float maxX = centerX + radius;
    float minZ = centerZ - radius;
    float maxZ = centerZ + radius;

    int gridMinX = clampInt(worldToGridX(minX), 0, GRID_SIZE - 1);
    int gridMaxX = clampInt(worldToGridX(maxX), 0, GRID_SIZE - 1);
    int gridMinZ = clampInt(worldToGridZ(minZ), 0, GRID_SIZE - 1);
    int gridMaxZ = clampInt(worldToGridZ(maxZ), 0, GRID_SIZE - 1);

    float radiusSq = radius * radius;
    for (int gx = gridMinX; gx <= gridMaxX; ++gx) {
        for (int gz = gridMinZ; gz <= gridMaxZ; ++gz) {
            float cellX = gridToWorldX(gx);
            float cellZ = gridToWorldZ(gz);
            float dx = cellX - centerX;
            float dz = cellZ - centerZ;
            if (dx * dx + dz * dz <= radiusSq) {
                grid[gx][gz].blocked = true;
            }
        }
    }
}
