#pragma once

#include "path_finder.h"

class BFSPathfinder final : public Pathfinder
{
public:
    void next_step() override;
};
