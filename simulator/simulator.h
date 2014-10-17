#pragma once
#include <system.h>
#include <settings.h>
#include <thermostat.h>
#include <statisticssampler.h>

class Simulator
{
public:
    System m_system;
    Settings m_settings;
    Thermostat *m_thermostat;
    StatisticsSampler *m_sampler;
    Simulator();
    void step();
};
