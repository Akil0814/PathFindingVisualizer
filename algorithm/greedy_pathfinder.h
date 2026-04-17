#pragma once

#include "path_finder.h"

class GreedyPathfinder final : public Pathfinder
{
public:
    void next_step() override;
};
