#ifndef BERENDSENTHERMOSTAT_H
#define BERENDSENTHERMOSTAT_H
class System;
class StatisticsSampler;

class BerendsenThermostat
{
private:
    float m_relaxationFactor;
public:
    BerendsenThermostat(float relaxationFactor);
    void apply(System *system, StatisticsSampler *sampler, float temperature);
};

#endif // BERENDSENTHERMOSTAT_H
