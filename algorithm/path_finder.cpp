#include "path_finder.h"

#include <algorithm>
#include <cmath>

bool Pathfinder::read_endpoints(Point& start, Point& goal) const
{
    // Every pathfinder begins by reading and validating the board endpoints.
    Board* current_board = board();
    if (current_board == nullptr)
        return false;

    start = current_board->get_start_point();
    goal = current_board->get_end_point();

    return current_board->in_bounds(start) && current_board->in_bounds(goal);
}

std::vector<Point> Pathfinder::neighbors(Point point) const
{
    // Delegate movement rules to Board so all algorithms share diagonal policy and wall filtering.
    Board* current_board = board();
    if (current_board == nullptr)
        return {};

    return current_board->neighbors(point, move_mode(), diagonal_policy());
}

bool Pathfinder::same_point(Point lhs, Point rhs) const
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool Pathfinder::is_start_or_goal(Point point) const
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point))
        return false;

    const Tile::Status status = current_board->tile_at(point).get_status();
    return status == Tile::Status::Start || status == Tile::Status::Goal;
}

int Pathfinder::tile_weight(Point point) const
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point))
        return 0;

    return current_board->tile_at(point)._weight;
}

int Pathfinder::movement_cost(Point from, Point to) const
{
    const int dx = std::abs(from.x - to.x);
    const int dy = std::abs(from.y - to.y);
    const bool diagonal = dx != 0 && dy != 0;
    const int base_cost = diagonal ? 14 : 10;

    return base_cost * tile_weight(to);
}

int Pathfinder::heuristic_cost(Point from, Point to, HeuristicMode mode) const
{
    // Heuristics use the same 10/14 scale as movement cost.
    const int dx = std::abs(from.x - to.x);
    const int dy = std::abs(from.y - to.y);

    switch (mode)
    {
    case HeuristicMode::Manhattan:
        return 10 * (dx + dy);

    case HeuristicMode::Euclidean:
        return static_cast<int>(std::round(10.0 * std::sqrt(dx * dx + dy * dy)));

    case HeuristicMode::Octile:
        return 14 * std::min(dx, dy) + 10 * (std::max(dx, dy) - std::min(dx, dy));

    case HeuristicMode::Chebyshev:
        return 10 * std::max(dx, dy);

    default:
        return 0;
    }
}

void Pathfinder::clear_tile_path_data(Point point)
{
    // Reset per-search metadata before using a tile as a new search root.
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point))
        return;

    Tile& tile = current_board->tile_at(point);
    tile.clear_parent();
    tile._g_cost = 0;
    tile._h_cost = 0;
    tile._f_cost = 0;
}

void Pathfinder::set_tile_parent(Point child, Point parent)
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(child))
        return;

    current_board->tile_at(child).set_parent(parent);
}

void Pathfinder::set_tile_costs(Point point, int g_cost, int h_cost)
{
    // Store costs on the tile so the visual debug panel can show algorithm progress.
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point))
        return;

    Tile& tile = current_board->tile_at(point);
    tile._g_cost = g_cost;
    tile._h_cost = h_cost;
    tile._f_cost = g_cost + h_cost;
}

void Pathfinder::mark_tile_current(Point point)
{
    // Start and goal keep their special icons, so visualization markers skip them.
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point) || is_start_or_goal(point))
        return;

    current_board->tile_at(point).change_status(Tile::Status::Current);
}

void Pathfinder::mark_tile_open(Point point)
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point) || is_start_or_goal(point))
        return;

    current_board->tile_at(point).change_status(Tile::Status::Open);
}

void Pathfinder::mark_tile_closed(Point point)
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point) || is_start_or_goal(point))
        return;

    current_board->tile_at(point).change_status(Tile::Status::Closed);
}

void Pathfinder::mark_tile_path(Point point)
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(point) || is_start_or_goal(point))
        return;

    current_board->tile_at(point).change_status(Tile::Status::Path);
}

void Pathfinder::close_current_tile(Point& current)
{
    // A visual step has finished expanding this tile; mark it closed and clear the tracker.
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(current))
        return;

    mark_tile_closed(current);
    current = { -1, -1 };
}

bool Pathfinder::rebuild_path(Point start, Point goal)
{
    // Follow parent links backward from goal to start and mark only intermediate tiles as path.
    Board* current_board = board();
    if (current_board == nullptr)
        return false;

    Point current = goal;
    int guard = current_board->row_count() * current_board->col_count();

    while (!same_point(current, start) && guard-- > 0)
    {
        if (!current_board->in_bounds(current))
            return false;

        const Point parent = current_board->tile_at(current).get_parent();
        if (!same_point(current, goal))
            mark_tile_path(current);

        current = parent;
    }

    return same_point(current, start);
}
