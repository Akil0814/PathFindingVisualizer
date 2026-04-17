#pragma once
#include <SDL.h>
#include"../Aframework/board.h"
#include "../status.h"

#include <memory>

class Pathfinder
{
public:
    virtual ~Pathfinder() = default;

    void bind_board(Board* board)
    {
        _board = board;
    }

    void set_move_mode(MoveMode move_mode)
    {
        _move_mode = move_mode;
    }

    virtual void next_step() = 0;
    [[nodiscard]] virtual std::unique_ptr<Pathfinder> clone() const = 0;

    bool is_finished() const
    {
        return _finished;
    }

    bool found_path() const
    {
        return _found_path;
    }

protected:
    [[nodiscard]] Board* board() const
    {
        return _board;
    }

    void mark_finished(bool found_path)
    {
        _finished = true;
        _found_path = found_path;
    }

    void reset_result()
    {
        _finished = false;
        _found_path = false;
    }

    [[nodiscard]] MoveMode move_mode() const
    {
        return _move_mode;
    }

	Board* _board = nullptr;
    MoveMode _move_mode = MoveMode::FourWay;

	bool _finished = false;
	bool _found_path = false;
};
