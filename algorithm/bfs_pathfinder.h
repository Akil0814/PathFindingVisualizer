#pragma once

#include "path_finder.h"

#include <queue>
#include <vector>

class BFSPathfinder final : public Pathfinder
{
public:
    void next_step() override;
    [[nodiscard]] std::unique_ptr<Pathfinder> clone() const override
    {
        return std::make_unique<BFSPathfinder>(*this);
    }

private:
    void initialize();
    void close_current_tile();
    void rebuild_path();
    [[nodiscard]] bool same_point(Point lhs, Point rhs) const;

private:
    std::queue<Point> _frontier;
    std::vector<std::vector<bool>> _visited;
    Point _start{ -1, -1 };
    Point _goal{ -1, -1 };
    Point _current{ -1, -1 };
    bool _initialized = false;
};
