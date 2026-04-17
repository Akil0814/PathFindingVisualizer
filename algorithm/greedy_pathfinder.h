#pragma once

#include "path_finder.h"

class GreedyPathfinder final : public Pathfinder
{
public:
    void next_step() override;
    [[nodiscard]] std::unique_ptr<Pathfinder> clone() const override
    {
        return std::make_unique<GreedyPathfinder>(*this);
    }
};
