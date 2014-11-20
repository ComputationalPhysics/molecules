#include "potentials/lennardjones.h"
#include "celllist.h"
#include "neighborlist.h"
#include "cpelapsedtimer.h"
#include <cmath>
#include <iostream>
using namespace std;
LennardJones::LennardJones(float sigma, float epsilon, float cutoffRadius) :
    m_sigma(sigma),
    m_sigma6(pow(sigma, 6.0)),
    m_epsilon(epsilon),
    m_rCutSquared(cutoffRadius*cutoffRadius),
    m_timeSinceLastNeighborListUpdate(0),
    m_potentialEnergyAtRcut(0)
{
    float oneOverDrCut2 = 1.0/m_rCutSquared;
    float oneOverDrCut6 = oneOverDrCut2*oneOverDrCut2*oneOverDrCut2;

    m_potentialEnergyAtRcut = 4*m_epsilon*m_sigma6*oneOverDrCut6*(m_sigma6*oneOverDrCut6 - 1);
}

void LennardJones::calculateForces(System *system)
{
    m_potentialEnergy = 0;
    m_pressureVirial = 0;
    vec3 systemSize = system->systemSize();

    if(!m_timeSinceLastNeighborListUpdate || m_timeSinceLastNeighborListUpdate++ > 20) {
        system->neighborList().update();
        m_timeSinceLastNeighborListUpdate = 1;
    }

    CPElapsedTimer::calculateForces().start();
    for(unsigned int i=0; i<system->atoms().size(); i++) {
        Atom *atom1 = system->atoms()[i];

        vector<Atom*> &neighbors = system->neighborList().neighborsForAtomWithIndex(atom1->index());

        for(unsigned int j=0; j<neighbors.size(); j++) {
            Atom *atom2 = neighbors[j];

            vec3 deltaRVector = atom1->position - atom2->position;

            // Minimum image convention
            deltaRVector = atom1->position - atom2->position;
            deltaRVector[0] +=systemSize[0]*((deltaRVector[0] < -0.5*systemSize[0]) - (deltaRVector[0] > 0.5*systemSize[0]));
            deltaRVector[1] +=systemSize[1]*((deltaRVector[1] < -0.5*systemSize[1]) - (deltaRVector[1] > 0.5*systemSize[1]));
            deltaRVector[2] +=systemSize[2]*((deltaRVector[2] < -0.5*systemSize[2]) - (deltaRVector[2] > 0.5*systemSize[2]));

            float dr2 = deltaRVector.lengthSquared();
            // float dr2 = dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2];
            float oneOverDr2 = 1.0/dr2;
            float oneOverDr6 = oneOverDr2*oneOverDr2*oneOverDr2;

            float force = -24*m_epsilon*m_sigma6*oneOverDr6*(2*m_sigma6*oneOverDr6 - 1)*oneOverDr2 * (dr2 < m_rCutSquared);

            atom1->force.addAndMultiply(deltaRVector, -force);
            atom2->force.addAndMultiply(deltaRVector, force);

            m_pressureVirial += force*sqrt(dr2)*dr2;
            m_potentialEnergy += (4*m_epsilon*m_sigma6*oneOverDr6*(m_sigma6*oneOverDr6 - 1) - m_potentialEnergyAtRcut)*(dr2 < m_rCutSquared);
        }
    }

    CPElapsedTimer::calculateForces().stop();
}
