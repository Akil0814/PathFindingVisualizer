#pragma once

#include "path_finder.h"

class AStarPathfinder final : public Pathfinder
{
public:
    void next_step() override;
};
