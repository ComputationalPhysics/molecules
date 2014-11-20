#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "simulator/system.h"
#include "simulator/modifiers/berendsenthermostat.h"
#include "simulator/statisticssampler.h"

class Simulator
{
private:
    bool m_thermostatEnabled;
    float m_temperature;
public:
    System m_system;
    BerendsenThermostat *m_thermostat;
    StatisticsSampler *m_sampler;
    Simulator();
    void step();
    bool thermostatEnabled() const;
    void setThermostatEnabled(bool thermostatEnabled);
    float temperature() const;
    void setTemperature(float temperature);
};

#endif // SIMULATOR_H
