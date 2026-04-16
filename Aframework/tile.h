#pragma once
#define SIZE_TILE 34

class Tile
{
public:
    enum class Status {
        Empty,      // Walkable tile
        Wall,       // Blocked tile
        Start,      // Starting point
        Goal,       // Target point
        Open,       // Discovered but not processed yet
        Closed,     // Already processed
        Path        // Final path
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

private:

    Status _status = Status::Empty;
    Tile* _parent = nullptr;

};