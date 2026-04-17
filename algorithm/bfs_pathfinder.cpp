#include "bfs_pathfinder.h"

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
        rebuild_path();
        return;
    }

    Tile& current_tile = current_board->tile_at(current);
    const Tile::Status current_status = current_tile.get_status();
    if (current_status != Tile::Status::Start && current_status != Tile::Status::Goal)
        current_tile.change_status(Tile::Status::Current);

    _current = current;

    for (const Point next : current_board->neighbors(current, move_mode()))
    {
        if (_visited[next.y][next.x])
            continue;

        _visited[next.y][next.x] = true;

        Tile& next_tile = current_board->tile_at(next);
        next_tile.set_parent(current);
        next_tile._g_cost = current_tile._g_cost + next_tile._weight;
        next_tile._h_cost = 0;
        next_tile._f_cost = next_tile._g_cost;

        const Tile::Status next_status = next_tile.get_status();
        if (next_status != Tile::Status::Start && next_status != Tile::Status::Goal)
            next_tile.change_status(Tile::Status::Open);

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

    _start = current_board->get_start_point();
    _goal = current_board->get_end_point();
    _current = { -1, -1 };

    if (!current_board->in_bounds(_start) || !current_board->in_bounds(_goal))
    {
        mark_finished(false);
        return;
    }

    _visited.assign(
        current_board->row_count(),
        std::vector<bool>(current_board->col_count(), false));

    while (!_frontier.empty())
        _frontier.pop();

    Tile& start_tile = current_board->tile_at(_start);
    start_tile.clear_parent();
    start_tile._g_cost = 0;
    start_tile._h_cost = 0;
    start_tile._f_cost = 0;

    _visited[_start.y][_start.x] = true;
    _frontier.push(_start);
}

void BFSPathfinder::close_current_tile()
{
    Board* current_board = board();
    if (current_board == nullptr || !current_board->in_bounds(_current))
        return;

    Tile& current_tile = current_board->tile_at(_current);
    if (current_tile.get_status() == Tile::Status::Current)
        current_tile.change_status(Tile::Status::Closed);

    _current = { -1, -1 };
}

void BFSPathfinder::rebuild_path()
{
    Board* current_board = board();
    if (current_board == nullptr)
    {
        mark_finished(false);
        return;
    }

    Point current = _goal;
    int guard = current_board->row_count() * current_board->col_count();

    while (!same_point(current, _start) && guard-- > 0)
    {
        if (!current_board->in_bounds(current))
        {
            mark_finished(false);
            return;
        }

        Tile& tile = current_board->tile_at(current);
        const Point parent = tile.get_parent();

        if (!same_point(current, _goal) && tile.get_status() != Tile::Status::Start)
            tile.change_status(Tile::Status::Path);

        current = parent;
    }

    mark_finished(same_point(current, _start));
}

bool BFSPathfinder::same_point(Point lhs, Point rhs) const
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
