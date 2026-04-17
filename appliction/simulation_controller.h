#pragma once
#include "../status.h"

class SimulationController
{
public:
    void on_update(double delta);

    void next_step();
    void pause();
    void restart();
    void set_auto_run(bool enabled);

    PlayMode get_play_mod();

private:
    PlayMode _current_play_mod = PlayMode::Idle;
    bool _auto_run = false;
    double _timer = 0.0;
    double _step_interval = 0.1;
};