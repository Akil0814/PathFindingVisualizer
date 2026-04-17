#pragma once

#include "path_finder.h"

class DijkstraPathfinder final : public Pathfinder
{
public:
    void next_step() override;
    [[nodiscard]] std::unique_ptr<Pathfinder> clone() const override
    {
        return std::make_unique<DijkstraPathfinder>(*this);
    }
};
