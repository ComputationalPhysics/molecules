#include "simulator.h"
#include <iostream>
using namespace std;

Simulator::Simulator()
{
    m_system.setup(&m_settings);

    m_thermostat = new Thermostat(m_settings.thermostat_relaxation_time);
    m_sampler = new StatisticsSampler(&m_system);
}

void Simulator::step() {

    m_system.sample_statistics = m_settings.statistics_interval && ((m_system.steps) % m_settings.statistics_interval == 0);
    m_system.step();
    if(m_system.sample_statistics) m_sampler->sample();
    if(m_settings.thermostat_enabled) m_thermostat->apply(m_sampler,&m_system,m_settings.temperature, false);
    if(m_settings.thermostat_frozen_enabled) m_thermostat->apply(m_sampler,&m_system,m_settings.temperature,true);
}
