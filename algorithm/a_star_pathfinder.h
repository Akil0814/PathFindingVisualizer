#pragma once

#include "path_finder.h"

#include <queue>
#include <vector>

class AStarPathfinder final : public CloneablePathfinder<AStarPathfinder>
{
public:
    void next_step() override;

    HeuristicMode _heuristic_mode = HeuristicMode::Manhattan;

private:
    struct QueueNode
    {
        Point point;
        int priority = 0;
        int heuristic = 0;
        int g_cost = 0;
    };

    struct QueueNodeCompare
    {
        bool operator()(const QueueNode& lhs, const QueueNode& rhs) const
        {
            if (lhs.priority != rhs.priority)
                return lhs.priority > rhs.priority;

            return lhs.heuristic > rhs.heuristic;
        }
    };

private:
    void initialize();

private:
    std::priority_queue<QueueNode, std::vector<QueueNode>, QueueNodeCompare> _open_set;
    std::vector<std::vector<int>> _best_cost;
    Point _start{ -1, -1 };
    Point _goal{ -1, -1 };
    Point _current{ -1, -1 };
    bool _initialized = false;
};
