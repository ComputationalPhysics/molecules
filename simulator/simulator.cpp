#include "simulator.h"


bool Simulator::thermostatEnabled() const
{
    return m_thermostatEnabled;
}

void Simulator::setThermostatEnabled(bool thermostatEnabled)
{
    m_thermostatEnabled = thermostatEnabled;
}

float Simulator::temperature() const
{
    return m_temperature;
}

void Simulator::setTemperature(float temperature)
{
    m_temperature = temperature;
}
Simulator::Simulator() :
    m_thermostatEnabled(false),
    m_temperature(300)
{
    m_thermostat = new BerendsenThermostat(300, 0.01);
    m_sampler = new StatisticsSampler();
}

void Simulator::step() {

    m_system.step();
    m_sampler->sample(&m_system);
    if(m_thermostatEnabled) m_thermostat->apply(&m_system, &m_sampler, m_temperature);
}
