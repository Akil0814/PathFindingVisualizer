#pragma once

#include "path_finder.h"

#include <queue>
#include <vector>

class GreedyPathfinder final : public CloneablePathfinder<GreedyPathfinder>
{
public:
    void next_step() override;

private:
    struct QueueNode
    {
        Point point;
        int heuristic = 0;
    };

    struct QueueNodeCompare
    {
        bool operator()(const QueueNode& lhs, const QueueNode& rhs) const
        {
            return lhs.heuristic > rhs.heuristic;
        }
    };

private:
    void initialize();
    void close_current_tile();
    [[nodiscard]] HeuristicMode default_heuristic_mode() const;

private:
    std::priority_queue<QueueNode, std::vector<QueueNode>, QueueNodeCompare> _open_set;
    std::vector<std::vector<bool>> _visited;
    Point _start{ -1, -1 };
    Point _goal{ -1, -1 };
    Point _current{ -1, -1 };
    bool _initialized = false;
};
