#include <cmath>
#include <atom.h>
#include <math/random.h>
#include <iostream>
using namespace std;

int Atom::totalNumberOfAtoms = 0;

Atom::Atom(float mass) :
    m_mass(mass),
    m_index(Atom::totalNumberOfAtoms++)
{

}

Atom::~Atom()
{

}

void Atom::resetForce()
{
    force.setToZero();
}

void Atom::resetVelocityMaxwellian(float temperature)
{
    // Resetting the velocity according to a Maxwell-Boltzmann distribution (see http://en.wikipedia.org/wiki/Maxwell%E2%80%93Boltzmann_distribution )
    float boltzmannConstant = 1.0; // In atomic units, the boltzmann constant equals 1
    float standardDeviation = sqrt(boltzmannConstant*temperature/m_mass);
    velocity.randomGaussian(0, standardDeviation);
}
