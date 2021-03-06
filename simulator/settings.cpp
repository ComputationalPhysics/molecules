#include <settings.h>
Settings::Settings() {
    FCC_b = 2.0;
    temperature = 1.0;
    dt = 0.02;
    r_cut = 2.0;
    thermostat_relaxation_time = 1;
    gravity_force = 0;
    mass = 39.948;
    gravity_direction = -1;
    unit_cells_x = 7;
    unit_cells_y = 7;
    unit_cells_z = 7;
    statistics_interval = 10;
    thermostat_enabled = false;
    thermostat_frozen_enabled = false;
}
