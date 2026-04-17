#pragma once
#include "../status.h"
#include "../algorithm/path_finder.h"
#include "../Aframework/board.h"

#include <memory>

class SimulationController
{
public:
    explicit SimulationController(Board* board = nullptr);

    void bind_board(Board* board);
    void on_update(double delta);

    void next_step();
    void pause();
    void restart();

    [[nodiscard]] int total_cost() const;
    [[nodiscard]] int total_steps() const;

    void set_auto_run(bool enabled);
    void set_auto_run_speed(double steps_per_second);
    void set_algorithm(Algorithm alg);


private:
    void create_path_finder();

private:
    PlayMode _current_play_mod = PlayMode::Idle;
    Algorithm _alg_using = Algorithm::AStart;

    Board* _board = nullptr;
    std::unique_ptr<Pathfinder> _path_finder;

    bool _auto_run = false;
    double _timer = 0.0;
    double _step_interval = 0.1;
    int _total_cost = 0;
    int _total_steps = 0;
};
