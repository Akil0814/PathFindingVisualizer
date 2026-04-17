#include "../greedy_pathfinder.h"

void GreedyPathfinder::next_step()
{
    if (is_finished())
        return;

    if (!_initialized)
        initialize();

    Board* current_board = board();
    if (current_board == nullptr || is_finished())
        return;

    close_current_tile();

    if (_open_set.empty())
    {
        mark_finished(false);
        return;
    }

    const Point current = _open_set.top().point;
    _open_set.pop();

    if (same_point(current, _goal))
    {
        mark_finished(rebuild_path(_start, _goal));
        return;
    }

    Tile& current_tile = current_board->tile_at(current);
    mark_tile_current(current);
    _current = current;

    const HeuristicMode heuristic_mode = default_heuristic_mode();
    for (const Point next : neighbors(current))
    {
        if (_visited[next.y][next.x])
            continue;

        _visited[next.y][next.x] = true;

        const int next_g_cost = current_tile._g_cost + tile_weight(next);
        const int next_h_cost = heuristic_cost(next, _goal, heuristic_mode);

        set_tile_parent(next, current);
        set_tile_costs(next, next_g_cost, next_h_cost);
        mark_tile_open(next);

        _open_set.push({ next, next_h_cost });
    }
}

void GreedyPathfinder::initialize()
{
    _initialized = true;
    _current = { -1, -1 };

    Board* current_board = board();
    if (current_board == nullptr || !read_endpoints(_start, _goal))
    {
        mark_finished(false);
        return;
    }

    _visited.assign(
        current_board->row_count(),
        std::vector<bool>(current_board->col_count(), false));

    while (!_open_set.empty())
        _open_set.pop();

    const int start_h_cost = heuristic_cost(_start, _goal, default_heuristic_mode());
    clear_tile_path_data(_start);
    set_tile_costs(_start, 0, start_h_cost);
    _visited[_start.y][_start.x] = true;
    _open_set.push({ _start, start_h_cost });
}

void GreedyPathfinder::close_current_tile()
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(_current))
        return;

    mark_tile_closed(_current);
    _current = { -1, -1 };
}

HeuristicMode GreedyPathfinder::default_heuristic_mode() const
{
    return move_mode() == MoveMode::EightWay ? HeuristicMode::Chebyshev : HeuristicMode::Manhattan;
}
