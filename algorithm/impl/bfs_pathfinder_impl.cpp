#include "../bfs_pathfinder.h"

void BFSPathfinder::next_step()
{
    if (is_finished())
        return;

    if (!_initialized)
        initialize();

    Board* current_board = board();
    if (current_board == nullptr || is_finished())
        return;

    close_current_tile();

    if (_frontier.empty())
    {
        mark_finished(false);
        return;
    }

    const Point current = _frontier.front();
    _frontier.pop();

    if (same_point(current, _goal))
    {
        mark_finished(rebuild_path(_start, _goal));
        return;
    }

    Tile& current_tile = current_board->tile_at(current);
    mark_tile_current(current);

    _current = current;

    for (const Point next : neighbors(current))
    {
        if (_visited[next.y][next.x])
            continue;

        _visited[next.y][next.x] = true;

        set_tile_parent(next, current);
        set_tile_costs(next, current_tile._g_cost + tile_weight(next), 0);
        mark_tile_open(next);

        _frontier.push(next);
    }
}

void BFSPathfinder::initialize()
{
    _initialized = true;

    Board* current_board = board();
    if (current_board == nullptr)
    {
        mark_finished(false);
        return;
    }

    _current = { -1, -1 };

    if (!read_endpoints(_start, _goal))
    {
        mark_finished(false);
        return;
    }

    _visited.assign(
        current_board->row_count(),
        std::vector<bool>(current_board->col_count(), false));

    while (!_frontier.empty())
        _frontier.pop();

    clear_tile_path_data(_start);

    _visited[_start.y][_start.x] = true;
    _frontier.push(_start);
}

void BFSPathfinder::close_current_tile()
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(_current))
        return;

    mark_tile_closed(_current);

    _current = { -1, -1 };
}
