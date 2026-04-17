#pragma once
#include "../status.h"
#include "../algorithm/path_finder.h"

class SimulationController
{
public:
    void on_update(double delta);

    void next_step();
    void pause();
    void restart();

    void set_auto_run(bool enabled);
    void set_algorithm(Algorithm alg);

    //PlayMode get_play_mod();

private:
    PlayMode _current_play_mod = PlayMode::Idle;
    bool _auto_run = false;
    double _timer = 0.0;
    double _step_interval = 0.1;
    int _total_cost = 0;
    int _total_steps = 0;
};