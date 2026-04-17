#pragma once

#include "path_finder.h"

#include <queue>
#include <vector>

class DijkstraPathfinder final : public CloneablePathfinder<DijkstraPathfinder>
{
public:
    void next_step() override;

private:
    struct QueueNode
    {
        Point point;
        int g_cost = 0;
    };

    struct QueueNodeCompare
    {
        bool operator()(const QueueNode& lhs, const QueueNode& rhs) const
        {
            return lhs.g_cost > rhs.g_cost;
        }
    };

private:
    void initialize();
    void close_current_tile();

private:
    std::priority_queue<QueueNode, std::vector<QueueNode>, QueueNodeCompare> _open_set;
    std::vector<std::vector<int>> _best_cost;
    Point _start{ -1, -1 };
    Point _goal{ -1, -1 };
    Point _current{ -1, -1 };
    bool _initialized = false;
};
