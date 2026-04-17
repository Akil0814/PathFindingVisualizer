#pragma once
#include "grid_point.h"

#define SIZE_TILE 34

class Tile
{
public:
    enum class Status
    {
        Empty,
        Wall,
        Start,
        Goal,
        Current,
        Open,
        Closed,
        Path
    };

    Tile() = default;

    void change_status(Status status)
    {
        _status = status;
    }

    Status get_status() const
    {
        return _status;
    }

    void set_parent(Point p)
    {
        _parent = p;
    }

    Point get_parent() const
    {
        return _parent;
    }

    void clear_parent()
    {
        _parent = { -1, -1 };
    }

public:
    int _g_cost = 0;
    int _h_cost = 0;
    int _f_cost = 0;
    int _weight = 1;
    Point _parent{ -1, -1 };

private:
    Status _status = Status::Empty;
};
