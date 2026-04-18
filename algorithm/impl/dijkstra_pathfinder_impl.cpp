#include "../dijkstra_pathfinder.h"

#include <limits>

void DijkstraPathfinder::next_step()
{
    // Dijkstra expands the tile with the lowest known cost from the start.
    if (is_finished())
        return;

    if (!_initialized)
        initialize();

    Board* current_board = board();
    if (current_board == nullptr || is_finished())
        return;

    // Finish the previously highlighted tile before selecting the next cheapest candidate.
    close_current_tile(_current);

    while (!_open_set.empty())
    {
        const QueueNode node = _open_set.top();
        _open_set.pop();

        // Skip stale queue entries left behind after a better path was found.
        if (node.g_cost != _best_cost[node.point.y][node.point.x])
            continue;

        const Point current = node.point;
        // With non-negative tile weights, popping the goal means its shortest cost is final.
        if (same_point(current, _goal))
        {
            mark_finished(rebuild_path(_start, _goal));
            return;
        }

        Tile& current_tile = current_board->tile_at(current);
        mark_tile_current(current);
        _current = current;

        // Relax neighbors using only accumulated g cost; Dijkstra has no heuristic.
        for (const Point next : neighbors(current))
        {
            const int next_g_cost = current_tile._g_cost + movement_cost(current, next);
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
    // best_cost starts at infinity and decreases as cheaper routes are discovered.
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

    // Start has zero accumulated cost and is the first queue entry.
    clear_tile_path_data(_start);
    _best_cost[_start.y][_start.x] = 0;
    _open_set.push({ _start, 0 });
}
