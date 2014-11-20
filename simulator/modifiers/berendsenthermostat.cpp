#include "berendsenthermostat.h"
#include "statisticssampler.h"
#include "system.h"
#include "atom.h"
#include <cmath>

BerendsenThermostat::BerendsenThermostat(float relaxationFactor) :
    m_relaxationFactor(relaxationFactor)
{
}

void BerendsenThermostat::apply(System *system, StatisticsSampler *sampler, float temperature)
{
    float scaleFactor = sqrt(1 + m_relaxationFactor*(sampler->temperature()/temperature - 1));
    for(int n=0; n<system->atoms().size(); n++) {
        Atom *atom = system->atoms()[n];
        atom->velocity *= scaleFactor;
    }
}
