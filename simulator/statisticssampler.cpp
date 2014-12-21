#include <statisticssampler.h>
#include <settings.h>
#include <unitconverter.h>
#include <system.h>
#include <iomanip>
#include <atom_types.h>
#include <string.h>

#include <iostream>
using namespace std;

StatisticsSampler::StatisticsSampler(System *system_) :
    temperature_sampled_at(-1),
    kinetic_energy_sampled_at(-1),
    pressure_sampled_at(-1),
    potential_energy_sampled_at(-1),
    count_periodic_sampled_at(-1)
{
    system = system_;
    settings = system->settings;

}

void StatisticsSampler::sample_momentum_cm() {
    double v_cm_local[3];

    v_cm[0] = 0; v_cm[1] = 0; v_cm[2] = 0;
    v_cm_local[0] = 0; v_cm_local[1] = 0; v_cm_local[2] = 0;

    for(unsigned int i=system->num_atoms_frozen;i<system->numAtoms();i++) {
        v_cm_local[0] += system->velocities[3*i+0];
        v_cm_local[1] += system->velocities[3*i+1];
        v_cm_local[2] += system->velocities[3*i+2];
    }

    p_cm[0] = v_cm[0]*settings->mass;
    p_cm[1] = v_cm[1]*settings->mass;
    p_cm[2] = v_cm[2]*settings->mass;
}

void StatisticsSampler::sample_kinetic_energy() {
    if(system->steps == kinetic_energy_sampled_at) return;
    kinetic_energy_sampled_at = system->steps;

    kinetic_energy = 0;

    for(unsigned int i=0;i<system->numAtoms();i++) {
        double vx = system->velocities[3*i+0];
        double vy = system->velocities[3*i+1];
        double vz = system->velocities[3*i+2];
        kinetic_energy += 0.5*settings->mass*(vx*vx + vy*vy + vz*vz);
    }
}

void StatisticsSampler::sample_potential_energy() {
    if(system->steps == potential_energy_sampled_at) return;
    potential_energy_sampled_at = system->steps;

    potential_energy = system->potentialEnergy();
}

void StatisticsSampler::sample_temperature() {
    if(system->steps == temperature_sampled_at) return;
    temperature_sampled_at = system->steps;

    sample_kinetic_energy();
    double kinetic_energy_per_atom = kinetic_energy / system->numAtoms();
    temperature = 2.0/3*kinetic_energy_per_atom;

    QVector3D freeAtomDrift;
    for(int n=0; n < int(system->numAtoms()); n++) {
        if(system->atom_type[n] == AtomType::Argon) {
            freeAtomDrift += QVector3D(system->velocities[3*n+0],
                    system->velocities[3*n+1],
                    system->velocities[3*n+2]);
        }
    }
    freeAtomDrift /= system->num_atoms_free;

    double kinetic_energy_free_atoms = 0;
    double kinetic_energy_frozen_atoms = 0;
    for(int n=0; n < int(system->numAtoms()); n++) {
        if(system->atom_type[n] == AtomType::Argon) {
            QVector3D relativeVelocity = QVector3D(system->velocities[3*n+0] - freeAtomDrift.x(),
                    system->velocities[3*n+1] - freeAtomDrift.y(),
                    system->velocities[3*n+2] - freeAtomDrift.z());
            double v_squared = relativeVelocity.lengthSquared();
            kinetic_energy_free_atoms += 0.5*system->settings->mass*v_squared;
        } else {
            double v_squared = system->velocities[3*n+0]*system->velocities[3*n+0] + system->velocities[3*n+1]*system->velocities[3*n+1] + system->velocities[3*n+2]*system->velocities[3*n+2];
            kinetic_energy_frozen_atoms += 0.5*system->settings->mass*v_squared;
        }
    }

    temperature_free_atoms = 2.0/(3*system->num_atoms_free)*kinetic_energy_free_atoms;
    temperature_frozen_atoms = 2.0/(3*system->num_atoms_frozen)*kinetic_energy_frozen_atoms;
}

void StatisticsSampler::sample_pressure() {
    if(system->steps == pressure_sampled_at) return;
    sample_temperature();

    pressure = system->pressure_forces;

    pressure /= 3*system->volume();
    pressure += system->num_atoms_free/system->volume()*temperature_free_atoms;

    pressure_sampled_at = system->steps;
}

void StatisticsSampler::sample() {
    double t_in_pico_seconds = system->unit_converter->time_to_SI(system->time())*1e12;
    sample_temperature();
    sample_potential_energy();
    sample_pressure();
    sample_velocity_distribution();
    sample_count_periodic();

    cout.setf(ios::fixed);
    cout.precision(5);
    double total_energy = kinetic_energy + potential_energy;
    cout << "Timestep " << setw(6) << system->steps << "   t=" << t_in_pico_seconds << " ps   T=" << system->unit_converter->temperature_to_SI(temperature) << " K   T(gas)=" << system->unit_converter->temperature_to_SI(temperature_free_atoms) << "K   E=" << system->unit_converter->energy_to_ev(total_energy) << " eV   Pot=" << system->unit_converter->energy_to_ev(potential_energy) << " eV" << endl;
}

void StatisticsSampler::sample_velocity_distribution() {

}

void StatisticsSampler::sample_count_periodic() {

}
