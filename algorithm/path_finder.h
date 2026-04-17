#pragma once
#include <SDL.h>
#include"../Aframework/board.h"
#include "../status.h"

#include <memory>
#include <vector>

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

    void set_diagonal_policy(DiagonalMovePolicy policy)
    {
        _diagonal_policy = policy;
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

    [[nodiscard]] DiagonalMovePolicy diagonal_policy()const
    {
        return _diagonal_policy;
    }

    [[nodiscard]] bool read_endpoints(Point& start, Point& goal) const;
    [[nodiscard]] std::vector<Point> neighbors(Point point) const;
    [[nodiscard]] bool same_point(Point lhs, Point rhs) const;
    [[nodiscard]] bool is_start_or_goal(Point point) const;
    [[nodiscard]] int tile_weight(Point point) const;
    [[nodiscard]] int heuristic_cost(Point from, Point to, HeuristicMode mode) const;

    void clear_tile_path_data(Point point);
    void set_tile_parent(Point child, Point parent);
    void set_tile_costs(Point point, int g_cost, int h_cost);
    void mark_tile_current(Point point);
    void mark_tile_open(Point point);
    void mark_tile_closed(Point point);
    void mark_tile_path(Point point);
    void close_current_tile(Point& current);
    [[nodiscard]] bool rebuild_path(Point start, Point goal);

	Board* _board = nullptr;
    MoveMode _move_mode = MoveMode::FourWay;
    DiagonalMovePolicy _diagonal_policy = DiagonalMovePolicy::BlockIfEitherSideBlocked;

	bool _finished = false;
	bool _found_path = false;
};

template <typename Derived>
class CloneablePathfinder : public Pathfinder
{
public:
    [[nodiscard]] std::unique_ptr<Pathfinder> clone() const override
    {
        return std::make_unique<Derived>(static_cast<const Derived&>(*this));
    }
};
