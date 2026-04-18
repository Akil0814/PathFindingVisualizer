#include "../a_star_pathfinder.h"

#include <limits>

void AStarPathfinder::next_step()
{
    // A* expands the tile with the smallest f = g + h score.
    if (is_finished())
        return;

    if (!_initialized)
        initialize();

    Board* current_board = board();
    if (current_board == nullptr || is_finished())
        return;

    // Finish the previously highlighted tile before selecting the next best candidate.
    close_current_tile(_current);

    while (!_open_set.empty())
    {
        const QueueNode node = _open_set.top();
        _open_set.pop();

        // Priority queues may contain older entries; ignore any that no longer match best_cost.
        if (node.g_cost != _best_cost[node.point.y][node.point.x])
            continue;

        const Point current = node.point;
        // The goal is optimal when it is popped from the open set with its best known g cost.
        if (same_point(current, _goal))
        {
            mark_finished(rebuild_path(_start, _goal));
            return;
        }

        Tile& current_tile = current_board->tile_at(current);
        mark_tile_current(current);
        _current = current;

        // Relax all neighbors: a lower g cost means this route is better.
        for (const Point next : neighbors(current))
        {
            const int next_g_cost = current_tile._g_cost + movement_cost(current, next);
            if (next_g_cost >= _best_cost[next.y][next.x])
                continue;

            const int next_h_cost = heuristic_cost(next, _goal, _heuristic_mode);
            _best_cost[next.y][next.x] = next_g_cost;

            set_tile_parent(next, current);
            set_tile_costs(next, next_g_cost, next_h_cost);
            mark_tile_open(next);

            _open_set.push({ next, next_g_cost + next_h_cost, next_h_cost, next_g_cost });
        }

        return;
    }

    mark_finished(false);
}

void AStarPathfinder::initialize()
{
    // best_cost stores the best g cost discovered for each tile.
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

    // Start enters the open set with g = 0 and h based on the selected heuristic.
    const int start_h_cost = heuristic_cost(_start, _goal, _heuristic_mode);
    clear_tile_path_data(_start);
    set_tile_costs(_start, 0, start_h_cost);
    _best_cost[_start.y][_start.x] = 0;
    _open_set.push({ _start, start_h_cost, start_h_cost, 0 });
}
