#include "simulation_controller.h"

#include "../algorithm/a_star_pathfinder.h"
#include "../algorithm/bfs_pathfinder.h"
#include "../algorithm/dijkstra_pathfinder.h"
#include "../algorithm/greedy_pathfinder.h"

SimulationController::SimulationController(Board* board)
    : _board(board)
{
    create_path_finder();
}

void SimulationController::bind_board(Board* board)
{
    _board = board;

    if (_path_finder != nullptr)
        _path_finder->bind_board(_board);
}

void SimulationController::on_update(double delta)
{
    if (!_auto_run || _current_play_mod != PlayMode::AutoRun)
        return;

    _timer += delta;
    if (_timer < _step_interval)
        return;

    _timer = 0.0;
    next_step();
}

void SimulationController::next_step()
{
    if (_board == nullptr)
        return;

    if (_path_finder == nullptr)
        create_path_finder();

    if (_path_finder == nullptr || _path_finder->is_finished())
    {
        set_auto_run(false);
        return;
    }

    _board->save_snapshot();
    _path_finder->next_step();
    ++_total_steps;

    if (_path_finder->is_finished())
        set_auto_run(false);
}

void SimulationController::pause()
{
    _auto_run = false;
    _current_play_mod = PlayMode::Pause;
    _timer = 0.0;

}

void SimulationController::restart()
{
    _auto_run = false;
    _current_play_mod = PlayMode::Idle;
    _timer = 0.0;
    _total_cost = 0;
    _total_steps = 0;

    if (_board != nullptr)
        _board->clear_path_data();

    create_path_finder();
}

int SimulationController::total_cost() const
{
    return _total_cost;
}

int SimulationController::total_steps() const
{
    return _total_steps;
}

void SimulationController::set_auto_run(bool enabled)
{
    _auto_run = enabled;
    _current_play_mod = enabled ? PlayMode::AutoRun : PlayMode::Pause;
    _timer = 0.0;

    if (enabled && _path_finder == nullptr)
        create_path_finder();
}

void SimulationController::set_auto_run_speed(double steps_per_second)
{
    if (steps_per_second < 1.0)
        steps_per_second = 1.0;

    _step_interval = 1.0 / steps_per_second;
}

void SimulationController::set_algorithm(Algorithm type)
{
    _alg_using = type;
    _timer = 0.0;
    create_path_finder();
}

void SimulationController::create_path_finder()
{
    switch (_alg_using)
    {
    case Algorithm::BFS:
        _path_finder = std::make_unique<BFSPathfinder>();
        break;
    case Algorithm::Dijkstar:
        _path_finder = std::make_unique<DijkstraPathfinder>();
        break;
    case Algorithm::AStart:
        _path_finder = std::make_unique<AStarPathfinder>();
        break;
    case Algorithm::Greedy:
        _path_finder = std::make_unique<GreedyPathfinder>();
        break;
    }

    if (_path_finder != nullptr)
        _path_finder->bind_board(_board);
}
