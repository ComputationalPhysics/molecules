#include "system.h"
#include "integrators/integrator.h"
#include "potentials/potential.h"
#include "statisticssampler.h"
#include "unitconverter.h"
#include "cpelapsedtimer.h"
#include <stdlib.h>

using namespace std;

System::System() :
    m_potential(0),
    m_integrator(0),
    m_currentTime(0),
    m_steps(0),
    m_initialized(false),
    m_ghostAtomsInUse(0)
{

}

System::~System()
{
    delete m_potential;
    delete m_integrator;
    m_atoms.clear();
}

void System::initialize(float cutoffRadius) {
    m_cellList.setup(this, cutoffRadius);
    m_neighborList.setup(this, cutoffRadius*1.2);
    m_initialized = true;
}

void System::applyPeriodicBoundaryConditions() {
    CPElapsedTimer::periodicBoundaryConditions().start();
    #pragma ivdep
    for(int i=0; i<m_atoms.size(); i++) {
        Atom *atom = m_atoms[i];
        for(int a=0; a<3; a++) {
            if(atom->position[a] < 0) atom->position[a] += m_systemSize[a];
            if(atom->position[a] >= m_systemSize[a]) atom->position[a] -= m_systemSize[a];
        }
    }
    CPElapsedTimer::periodicBoundaryConditions().stop();
    // Read here: http://en.wikipedia.org/wiki/Periodic_boundary_conditions#Practical_implementation:_continuity_and_the_minimum_image_convention
}

void System::removeMomentum() {
    // Initially, when the atoms are given random velocities, there is a non-zero net momentum. We don't want any drift in the system, so we need to remove it.
    StatisticsSampler sampler;
    vec3 momentum = sampler.sampleMomentum(this);

    momentum /= m_atoms.size();
    for(int i=0; i<m_atoms.size(); i++) {
        Atom *atom = m_atoms[i];
        atom->velocity -= momentum/atom->mass();
    }
}

void System::resetForcesOnAllAtoms() {
    #pragma ivdep
    for(int i=0; i<m_atoms.size(); i++) {
        Atom *atom = m_atoms[i];
        atom->resetForce();
    }
}

void System::createFCCLattice(int numberOfUnitCellsEachDimension, float latticeConstant, float temperature) {
    float xCell[4] = {0, 0.5, 0.5, 0};
    float yCell[4] = {0, 0.5, 0, 0.5};
    float zCell[4] = {0, 0, 0.5, 0.5};
    for(int i=0; i< numberOfUnitCellsEachDimension; i++) {
        for(int j=0; j< numberOfUnitCellsEachDimension; j++) {
            for(int k=0; k< numberOfUnitCellsEachDimension; k++) {
                for(int l=0; l<4; l++) {
                    Atom *atom = new Atom(UnitConverter::massFromSI(6.63352088e-26));
                    float x = (i+xCell[l])*latticeConstant;
                    float y = (j+yCell[l])*latticeConstant;
                    float z = (k+zCell[l])*latticeConstant;
                    atom->position.set(x,y,z);
                    atom->resetVelocityMaxwellian(temperature);
                    m_atoms.push_back(atom);
                }
            }
        }
    }

    float sideLength = numberOfUnitCellsEachDimension*latticeConstant;
    setSystemSize(vec3(sideLength, sideLength, sideLength));
    cout << "Added " << m_atoms.size() << " atoms in an FCC lattice." << endl;
}

void System::copyAtomToGhostAtom(Atom *atom) {

}

void System::createGhostAtoms() {
    float fraction = 0.02;
    for(int i=0; i<m_atoms.size()*fraction; i++) {
        Atom *atom = new Atom(UnitConverter::massFromSI(6.63352088e-26));
        m_ghostAtoms.push_back(atom);
    }
}

void System::calculateForces() {
    resetForcesOnAllAtoms();
    m_potential->calculateForces(this);
}

void System::step(float dt) {
    if(!m_initialized) {
        cout << "System not initialized, aborting. Remember to call initialize()." << endl;
        exit(1);
    }
    m_integrator->integrate(this, dt);
    m_steps++;
    m_currentTime += dt;
}
