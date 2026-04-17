#pragma once

#include "path_finder.h"

class DijkstraPathfinder final : public Pathfinder
{
public:
    void next_step() override;
};
