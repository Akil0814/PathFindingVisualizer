#include "../dijkstra_pathfinder.h"

#include <limits>

void DijkstraPathfinder::next_step()
{
    if (is_finished())
        return;

    if (!_initialized)
        initialize();

    Board* current_board = board();
    if (current_board == nullptr || is_finished())
        return;

    close_current_tile();

    while (!_open_set.empty())
    {
        const QueueNode node = _open_set.top();
        _open_set.pop();

        if (node.g_cost != _best_cost[node.point.y][node.point.x])
            continue;

        const Point current = node.point;
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
            const int next_g_cost = current_tile._g_cost + tile_weight(next);
            if (next_g_cost >= _best_cost[next.y][next.x])
                continue;

            _best_cost[next.y][next.x] = next_g_cost;

            set_tile_parent(next, current);
            set_tile_costs(next, next_g_cost, 0);
            mark_tile_open(next);

            _open_set.push({ next, next_g_cost });
        }

        return;
    }

    mark_finished(false);
}

void DijkstraPathfinder::initialize()
{
    _initialized = true;
    _current = { -1, -1 };

    Board* current_board = board();
    if (current_board == nullptr || !read_endpoints(_start, _goal))
    {
        mark_finished(false);
        return;
    }

    _best_cost.assign(
        current_board->row_count(),
        std::vector<int>(current_board->col_count(), std::numeric_limits<int>::max()));

    while (!_open_set.empty())
        _open_set.pop();

    clear_tile_path_data(_start);
    _best_cost[_start.y][_start.x] = 0;
    _open_set.push({ _start, 0 });
}

void DijkstraPathfinder::close_current_tile()
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(_current))
        return;

    mark_tile_closed(_current);
    _current = { -1, -1 };
}
