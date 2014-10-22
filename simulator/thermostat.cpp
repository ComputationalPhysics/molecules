#include <thermostat.h>
#include <system.h>
#include <statisticssampler.h>
#include <settings.h>
#include <math.h>
#include <iostream>
#include <unitconverter.h>
#include <atom_types.h>

using namespace std;

Thermostat::Thermostat(double relaxation_time_)
{
    relaxation_time = relaxation_time_;
}

void Thermostat::apply(StatisticsSampler *sampler, System *system, const double &temperature, int atomType) {
    if(atomType != -1) {

        int atomCount = 0;
        QVector3D freeAtomDrift;
        for(int n=0; n<system->numAtoms(); n++) {
            if(system->atom_type[n] == atomType) {
                freeAtomDrift += QVector3D(system->velocities[3*n+0],
                        system->velocities[3*n+1],
                        system->velocities[3*n+2]);
                atomCount++;
            }
        }
        freeAtomDrift /= atomCount;

        double kinetic_energy = 0;
        for(int n=0; n<system->numAtoms();n++) {
            if(system->atom_type[n]==atomType) {
                QVector3D relativeVelocity = QVector3D(system->velocities[3*n+0] - freeAtomDrift.x(),
                        system->velocities[3*n+1] - freeAtomDrift.y(),
                        system->velocities[3*n+2] - freeAtomDrift.z());
                double v_squared = relativeVelocity.lengthSquared();
                kinetic_energy += 0.5*system->settings->mass*v_squared;
            }
        }

        double kinetic_energy_per_atom = kinetic_energy / atomCount;
        double current_temperature = 2.0/3*kinetic_energy_per_atom;
        double berendsen_factor = sqrt(1 + system->dt()/relaxation_time*(temperature/current_temperature - 1));

        for(int n=0; n<system->numAtoms();n++) {
            if(system->atom_type[n]==atomType) {
                for(short a=0;a<3;a++) system->velocities[3*n+a] *= berendsen_factor;
            }
        }
    } else {
        sampler->sample_temperature();
        double berendsen_factor = sqrt(1 + system->dt()/relaxation_time*(temperature/sampler->temperature - 1));

        for(unsigned long n=0;n<system->numAtoms();n++) {
            if(system->atom_type[n] == FIXED) {
                return;
            }
            for(short a=0;a<3;a++) {
                system->velocities[3*n+a] *= berendsen_factor;
            }
        }
    }
}
