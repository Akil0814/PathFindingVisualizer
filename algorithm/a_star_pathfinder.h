#pragma once

#include "path_finder.h"

class AStarPathfinder final : public Pathfinder
{
public:
    enum class HeuristicMode
    {
        Manhattan,
        Euclidean,
        Octile,
        Chebyshev
    };

public:
    void next_step() override;

    HeuristicMode _heuristic_mode = HeuristicMode::Manhattan;
};
