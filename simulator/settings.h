#pragma once

class Settings
{
public:
    Settings();
    double temperature;
    double dt;
    double FCC_b;
    double r_cut;
    double thermostat_relaxation_time;
    double gravity_force;
    double mass;
    int gravity_direction;
    int unit_cells_x;
    int unit_cells_y;
    int unit_cells_z;
    int timesteps;
    int statistics_interval;
    bool thermostat_enabled;
    bool thermostat_frozen_enabled;
};
