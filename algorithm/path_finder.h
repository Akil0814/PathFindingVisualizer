#pragma once
#include <SDL.h>
#include"../Aframework/board.h"
#include "../status.h"

class Pathfinder
{
public:
    virtual ~Pathfinder() = default;

    void bind_board(Board* board)
    {
        _board = board;
    }

    virtual void next_step() = 0;

    bool is_finished() const
    {
        return _finished;
    }

    bool found_path() const
    {
        return _found_path;
    }

protected:
	Board* _board = nullptr;

	bool _finished = false;
	bool _found_path = false;
};