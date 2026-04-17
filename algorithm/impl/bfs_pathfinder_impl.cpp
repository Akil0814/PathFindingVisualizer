#include "../bfs_pathfinder.h"

void BFSPathfinder::next_step()
{
    // BFS explores in first-in-first-out order, so it expands the grid by layers.
    if (is_finished())
        return;

    if (!_initialized)
        initialize();

    Board* current_board = board();
    if (current_board == nullptr || is_finished())
        return;

    // Finish the previous visual step before choosing the next frontier tile.
    close_current_tile(_current);

    if (_frontier.empty())
    {
        mark_finished(false);
        return;
    }

    const Point current = _frontier.front();
    _frontier.pop();

    // The first time the goal is popped, BFS has found the shortest path in step count.
    if (same_point(current, _goal))
    {
        mark_finished(rebuild_path(_start, _goal));
        return;
    }

    Tile& current_tile = current_board->tile_at(current);
    mark_tile_current(current);

    _current = current;

    // Discover each unvisited neighbor once and remember how we reached it.
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
    // The queue stores discovered tiles waiting to be expanded.
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
