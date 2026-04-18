#include "simulation_controller.h"

#include "../algorithm/a_star_pathfinder.h"
#include "../algorithm/bfs_pathfinder.h"
#include "../algorithm/custom_pathfinder.h"
#include "../algorithm/dijkstra_pathfinder.h"
#include "../algorithm/greedy_pathfinder.h"

#include <algorithm>
#include <utility>
#include <iostream>

SimulationController::SimulationController(Board* board)
    : _board(board)
{
    if (_board != nullptr)
        _board->set_movement_cost_config(_movement_cost_config);

    create_path_finder();
}

void SimulationController::bind_board(Board* board)
{
    _board = board;

    if (_path_finder != nullptr)
        _path_finder->bind_board(_board);

    if (_board != nullptr)
    {
        _board->set_edit_locked(_board_edit_locked);
        _board->set_movement_cost_config(_movement_cost_config);
    }
}

void SimulationController::on_update(double delta)
{
    if (!_auto_run || _current_play_mod != PlayMode::AutoRun)
        return;

    _timer += delta;

    constexpr int max_steps_per_frame = 64;
    int steps_this_frame = 0;

    while (_timer >= _step_interval &&
        _auto_run &&
        _current_play_mod == PlayMode::AutoRun &&
        steps_this_frame < max_steps_per_frame)
    {
        _timer -= _step_interval;
        next_step();
        ++steps_this_frame;
    }

    if (steps_this_frame == max_steps_per_frame)
        _timer = 0.0;
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

    std::cout << "-------------next step--------------" << std::endl;

    set_board_edit_locked(true);
    save_history_state();
    _board->save_snapshot();
    _path_finder->next_step();
    ++_total_steps;

    if (_path_finder->is_finished())
    {
        _total_cost = _path_finder->found_path() ? _board->path_cost() : 0;
        _path_steps = _path_finder->found_path() ? _board->path_steps() : 0;
        set_auto_run(false);
    }
}

bool SimulationController::previous_step()
{
    if (_board == nullptr || _history.empty())
        return false;

    if (!_board->undo())
        return false;

    std::cout << "------------previous step-----------" << std::endl;

    HistoryState state = std::move(_history.back());
    _history.pop_back();

    _path_finder = std::move(state.path_finder);
    if (_path_finder != nullptr)
    {
        _path_finder->bind_board(_board);
        _path_finder->set_move_mode(_move_mode);
        _path_finder->set_diagonal_policy(_diagonal_policy);
    }

    _auto_run = false;
    _current_play_mod = PlayMode::Pause;
    _timer = 0.0;
    _total_cost = state.total_cost;
    _total_steps = state.total_steps;
    _path_steps = state.path_steps;

    set_board_edit_locked(true);
    return true;
}

void SimulationController::pause()
{
    if (!is_auto_running())
        return;
    std::cout<<"---------------pause---------------" << std::endl;
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
    _path_steps = 0;
    _history.clear();
    set_board_edit_locked(false);

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

int SimulationController::path_steps() const
{
    return _path_steps;
}

bool SimulationController::is_board_edit_locked() const
{
    return _board_edit_locked;
}

bool SimulationController::is_auto_running() const
{
    return _auto_run && _current_play_mod == PlayMode::AutoRun;
}

bool SimulationController::is_pathfinder_finished() const
{
    return _path_finder != nullptr && _path_finder->is_finished();
}

bool SimulationController::found_path() const
{
    return _path_finder != nullptr && _path_finder->found_path();
}

PlayMode SimulationController::play_mode() const
{
    return _current_play_mod;
}

SimState SimulationController::sim_state() const
{
    if (is_pathfinder_finished())
        return SimState::Finished;

    if (_board_edit_locked)
        return SimState::Running;

    return SimState::Editing;
}

Algorithm SimulationController::algorithm() const
{
    return _alg_using;
}

void SimulationController::set_auto_run(bool enabled)
{
    std::cout << "--------------auto run--------------" << std::endl;
    _auto_run = enabled;
    _current_play_mod = enabled ? PlayMode::AutoRun : PlayMode::Pause;
    _timer = 0.0;

    if (enabled)
    {
        set_board_edit_locked(true);
        if (_path_finder == nullptr)
            create_path_finder();
    }
}

void SimulationController::set_auto_run_speed(double steps_per_second)
{
    if (steps_per_second < 1.0)
        steps_per_second = 1.0;

    _step_interval = 1.0 / steps_per_second;
}

void SimulationController::set_algorithm(Algorithm type)
{
    if (_board_edit_locked)
        return;

    _alg_using = type;
    _timer = 0.0;
    create_path_finder();
}

void SimulationController::set_move_mode(MoveMode move_mode)
{
    if (_board_edit_locked)
        return;

    _move_mode = move_mode;

    if (_path_finder != nullptr)
        _path_finder->set_move_mode(_move_mode);
}

void SimulationController::set_diagonal_policy(DiagonalMovePolicy policy)
{
    if (_board_edit_locked)
        return;

    _diagonal_policy = policy;

    if (_path_finder != nullptr)
        _path_finder->set_diagonal_policy(_diagonal_policy);
}

void SimulationController::set_movement_cost_config(MovementCostConfig config)
{
    if (_board_edit_locked)
        return;

    config.straight = std::max(1, config.straight);
    config.diagonal = std::max(1, config.diagonal);
    _movement_cost_config = config;

    if (_board != nullptr)
        _board->set_movement_cost_config(_movement_cost_config);

    create_path_finder();
}

void SimulationController::set_a_star_heuristic(HeuristicMode heuristic_mode)
{
    if (_board_edit_locked)
        return;

    _a_star_heuristic = heuristic_mode;

    if (_alg_using == Algorithm::AStar)
        create_path_finder();
}

MoveMode SimulationController::move_mode() const
{
    return _move_mode;
}

DiagonalMovePolicy SimulationController::diagonal_policy() const
{
    return _diagonal_policy;
}

MovementCostConfig SimulationController::movement_cost_config() const
{
    return _movement_cost_config;
}

HeuristicMode SimulationController::a_star_heuristic() const
{
    return _a_star_heuristic;
}

void SimulationController::set_board_edit_locked(bool locked)
{
    _board_edit_locked = locked;

    if (_board != nullptr)
        _board->set_edit_locked(locked);
}

void SimulationController::create_path_finder()
{
    switch (_alg_using)
    {
    case Algorithm::BFS:
        _path_finder = std::make_unique<BFSPathfinder>();
        break;
    case Algorithm::Dijkstra:
        _path_finder = std::make_unique<DijkstraPathfinder>();
        break;
    case Algorithm::AStar:
    {
        auto finder = std::make_unique<AStarPathfinder>();
        finder->_heuristic_mode = _a_star_heuristic;
        _path_finder = std::move(finder);
        break;
    }
    case Algorithm::Greedy:
        _path_finder = std::make_unique<GreedyPathfinder>();
        break;
    case Algorithm::Custom:
        _path_finder = std::make_unique<CustomPathfinder>();
        break;
    }

    if (_path_finder != nullptr)
    {
        _path_finder->bind_board(_board);
        _path_finder->set_move_mode(_move_mode);
        _path_finder->set_diagonal_policy(_diagonal_policy);
    }
}

void SimulationController::save_history_state()
{
    HistoryState state;
    state.path_finder = _path_finder != nullptr ? _path_finder->clone() : nullptr;
    state.play_mode = _current_play_mod;
    state.timer = _timer;
    state.total_cost = _total_cost;
    state.total_steps = _total_steps;
    state.path_steps = _path_steps;

    _history.push_back(std::move(state));
}
