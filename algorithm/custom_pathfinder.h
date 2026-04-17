#pragma once

#include "path_finder.h"

class CustomPathfinder final : public CloneablePathfinder<CustomPathfinder>
{
public:
    void next_step() override;
};
