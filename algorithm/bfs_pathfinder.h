#pragma once

#include "path_finder.h"

#include <queue>
#include <vector>

class BFSPathfinder final : public CloneablePathfinder<BFSPathfinder>
{
public:
    void next_step() override;

private:
    void initialize();

private:
    std::queue<Point> _frontier;
    std::vector<std::vector<bool>> _visited;
    Point _start{ -1, -1 };
    Point _goal{ -1, -1 };
    Point _current{ -1, -1 };
    bool _initialized = false;
};
