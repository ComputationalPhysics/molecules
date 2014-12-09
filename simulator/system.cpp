#include <iostream>
#include <cmath>
#include <time.h>
#include <fstream>
#include <system.h>
#include <settings.h>
#include <unitconverter.h>
#include <random.h>
#include <statisticssampler.h>
#include <atom_types.h>
#include <algorithm>
#include <string.h>
#include <QDebug>

#include "mdio.h"
#include "potential_lennard_jones.h"

using namespace std;
System::System() :
    numberOfPrecomputedTwoParticleForces(0),
    deltaR2(0),
    oneOverDeltaR2(0),
    cell_index(0),
    cell_index_2(0),
    num_cells_including_ghosts_xyz(0),
    m_rCut(0),
    m_dt(0),
    m_potentialEnergy(0),
    m_time(0),
    m_startTime(0),
    one_over_r_cut_squared(0),
    settings(0),
    mdio(0),
    rnd(0),
    unit_converter(0),
    sample_statistics(0),
    steps(0),
    m_numAtoms(0),
    num_atoms_free(0),
    num_atoms_fixed(0),
    num_atoms_frozen(0),
    num_atoms_ghost(0),
    mass_inverse(0),
    pressure_forces(0),
    m_didScaleVelocitiesDueToHighValues(false),
    m_systemSize(-999, -999, -999)
{
    num_cells[0] = 0;
    num_cells[1] = 0;
    num_cells[2] = 0;
    num_cells_including_ghosts[0] = 0;
    num_cells_including_ghosts[1] = 0;
    num_cells_including_ghosts[2] = 0;
    shift_vector[0][0] = 0;
    shift_vector[0][1] = 0;
    shift_vector[0][2] = 0;
    shift_vector[1][0] = 0;
    shift_vector[1][1] = 0;
    shift_vector[1][2] = 0;
    shift_vector[2][0] = 0;
    shift_vector[2][1] = 0;
    shift_vector[2][2] = 0;
    shift_vector[3][0] = 0;
    shift_vector[3][1] = 0;
    shift_vector[3][2] = 0;
    shift_vector[4][0] = 0;
    shift_vector[4][1] = 0;
    shift_vector[4][2] = 0;
    shift_vector[5][0] = 0;
    shift_vector[5][1] = 0;
    shift_vector[5][2] = 0;
    mc[0] = 0;
    mc[1] = 0;
    mc[2] = 0;
    mc1[0] = 0;
    mc1[1] = 0;
    mc1[2] = 0;
    count_periodic[0] = 0;
    count_periodic[1] = 0;
    count_periodic[2] = 0;
}

void System::createForcesAndPotentialTable() {
    numberOfPrecomputedTwoParticleForces = 8192;

    precomputed_forces.resize(numberOfPrecomputedTwoParticleForces+1);
    precomputed_potential.resize(numberOfPrecomputedTwoParticleForces+1);

    double rMinSquared = 0;
    double rMaxSquared = m_rCut*m_rCut;

    deltaR2 = (rMaxSquared - rMinSquared) / (numberOfPrecomputedTwoParticleForces-1);
    oneOverDeltaR2 = 1.0/deltaR2;

    for(int i=0; i<=numberOfPrecomputedTwoParticleForces; i++) {
        double r2 = rMinSquared + i*deltaR2;

        if(r2 > m_rCut*m_rCut) continue;
        double r = sqrt(r2);
        double oneOverR2 = 1.0/r2;
        double oneOverR6 = oneOverR2*oneOverR2*oneOverR2;

        double oneOverRCut2 = 1.0/(m_rCut*m_rCut);
        double oneOverRCut6 = oneOverRCut2*oneOverRCut2*oneOverRCut2;

        double force = 24*(2*oneOverR6-1)*oneOverR6*oneOverR2*mass_inverse;
        double forceAtRCut = 24*(2*oneOverRCut6-1)*oneOverRCut6*oneOverRCut2*mass_inverse;

        double potential = 4*oneOverR6*(oneOverR6 - 1);
        double potentialAtRCut = 4*oneOverRCut6*(oneOverRCut6 - 1);

        double potentialShifted = potential - potentialAtRCut + (r - m_rCut)*forceAtRCut;
        double forceShifted = force - forceAtRCut;

        precomputed_forces[i] = forceShifted;
        precomputed_potential[i] = potentialShifted;
    }
}
bool System::didScaleVelocitiesDueToHighValues() const
{
    return m_didScaleVelocitiesDueToHighValues;
}

void System::setDidScaleVelocitiesDueToHighValues(bool didScaleVelocitiesDueToHighValues)
{
    m_didScaleVelocitiesDueToHighValues = didScaleVelocitiesDueToHighValues;
}

unsigned long System::numAtoms() const
{
    return m_numAtoms;
}

void System::setNumAtoms(unsigned long numAtoms)
{
    m_numAtoms = numAtoms;
    allocate(27*m_numAtoms);
}

double System::startTime() const
{
    return m_startTime;
}

void System::setStartTime(double startTime)
{
    m_startTime = startTime;
}

double System::time() const
{
    return m_time;
}

void System::setTime(double time)
{
    m_time = time;
}

double System::potentialEnergy() const
{
    return m_potentialEnergy;
}

double System::dt() const
{
    return m_dt;
}

void System::setDt(double dt)
{
    m_dt = dt;
}

double System::rCut() const
{
    return m_rCut;
}

void System::setRCut(double rCut, bool autoResetCells)
{
    m_rCut = rCut;
    createForcesAndPotentialTable();
    if(autoResetCells) {
        resetCells();
    }
}

QVector3D System::systemSize() const
{
    return m_systemSize;
}

void System::setSystemSize(const QVector3D &systemSize, bool autoResetCells)
{
    if(m_systemSize == systemSize) {
        return;
    }

    float scaleX = m_systemSize.x() == 0 ? 1 : systemSize.x()/m_systemSize.x();
    float scaleY = m_systemSize.y() == 0 ? 1 : systemSize.y()/m_systemSize.y();
    float scaleZ = m_systemSize.z() == 0 ? 1 : systemSize.z()/m_systemSize.z();

    for(int n=0; n<int(m_numAtoms); n++) {
        positions[3*n+0] *= scaleX;
        positions[3*n+1] *= scaleY;
        positions[3*n+2] *= scaleZ;
        initial_positions[3*n+0] *= scaleX;
        initial_positions[3*n+1] *= scaleY;
        initial_positions[3*n+2] *= scaleZ;
    }

    m_systemSize = systemSize;

    if(autoResetCells) {
        resetCells();
    }
}

void System::resetCells() {
    for(int a=0;a<3;a++) {
        num_cells[a] = m_systemSize[a]/m_rCut;
        num_cells_including_ghosts[a] = num_cells[a]+2;

        cellLength[a] = m_systemSize[a]/num_cells[a];
        count_periodic[a] = 0;
    }

    num_cells_including_ghosts_yz = num_cells_including_ghosts[1]*num_cells_including_ghosts[2];
    num_cells_including_ghosts_xyz = num_cells_including_ghosts_yz*num_cells_including_ghosts[0];
    head_all_atoms.resize(num_cells_including_ghosts_xyz);
    head_free_atoms.resize(num_cells_including_ghosts_xyz);
    int numberOfCells = (num_cells[0]+2)*(num_cells[1]+2)*(num_cells[2]+2);
    if(numberOfCells > int(is_ghost_cell.size())) {
        is_ghost_cell.resize(numberOfCells);
    }

    for(unsigned long cx=0;cx<num_cells[0]+2;cx++) {
        for(unsigned long cy=0;cy<num_cells[1]+2;cy++) {
            for(unsigned long cz=0;cz<num_cells[2]+2;cz++) {
                cell_index_from_ijk(cx,cy,cz,cell_index);
                if(cx == 0 || cx == num_cells[0]+1 || cy == 0 || cy == num_cells[1]+1 || cz == 0 || cz == num_cells[2]+1) {
                    is_ghost_cell[cell_index] = true;
                } else is_ghost_cell[cell_index] = false;
            }
        }
    }

    set_topology();
}

double System::volume()
{
    return m_systemSize[0]*m_systemSize[1]*m_systemSize[2];
}

void System::allocate(int numberOfAtoms) {
    positions.resize(3*numberOfAtoms);
    accelerations.resize(3*numberOfAtoms);
    velocities.resize(3*numberOfAtoms);
    atom_type.resize(numberOfAtoms, 0);
    atom_moved.resize(numberOfAtoms, false);
    m_dataBuffer.resize(numberOfAtoms);
    atom_ids.resize(numberOfAtoms);

    linked_list_all_atoms.resize(numberOfAtoms);
    linked_list_free_atoms.resize(numberOfAtoms);
    initial_positions.resize(3*numberOfAtoms);
    move_queue.resize(6);
    for(int i=0; i<6; i++) {
        move_queue[i].resize(numberOfAtoms);
    }
}

void System::setup(Settings *settings_) {
    unit_converter = new UnitConverter();

    settings = settings_;
    m_numAtoms = 0;
    num_atoms_ghost = 0;

    steps = 0;
    int seed = -1;
    rnd = new Random(seed);

    init_parameters();
    createForcesAndPotentialTable();

    mdio = new MDIO();
    mdio->setup(this);

    set_topology();
    create_FCC();

    countAtomTypes();

    mpi_copy();
    calculate_accelerations();

    cout << "System size: " << unit_converter->length_to_SI(m_systemSize[0])*1e10 << " Å " << unit_converter->length_to_SI(m_systemSize[1])*1e10 << " Å " << unit_converter->length_to_SI(m_systemSize[2])*1e10 << " Å" << endl;
    cout << "Atoms: " << m_numAtoms << endl;
    cout << "Free atoms: " << num_atoms_free << endl;
}

void System::countAtomTypes() {
    num_atoms_free = 0;
    num_atoms_fixed = 0;
    num_atoms_frozen = 0;
    for(int i=0; i<int(m_numAtoms); i++) {
        if(atom_type[i] == FROZEN)  {
            num_atoms_frozen++;
        } else if(atom_type[i] == FIXED) {
            num_atoms_fixed++;
        } else if(atom_type[i] == ARGON) {
            num_atoms_free++;
        } else {
            cout << "WARNING: Unknown atom type in count_frozen_atoms: " << atom_type[i] << endl;
        }
    }
}

void System::create_FCC() {
    double xCell[4] = {0, 0.5, 0.5, 0};
    double yCell[4] = {0, 0.5, 0, 0.5};
    double zCell[4] = {0, 0, 0.5, 0.5};

    double r[3];
    double T = settings->temperature;

    int numberOfAtoms = settings->unit_cells_x*settings->unit_cells_y*settings->unit_cells_z*4;
    setNumAtoms(numberOfAtoms);

    int currentAtomIndex = 0;
    for(int x = 0; x < settings->unit_cells_x; x++) {
        for(int y = 0; y < settings->unit_cells_y; y++) {
            for(int z = 0; z < settings->unit_cells_z; z++) {
                for(int k = 0; k < 4; k++) {
                    // Set positions and type

                    r[0] = (x+xCell[k]) * settings->FCC_b;
                    r[1] = (y+yCell[k]) * settings->FCC_b;
                    r[2] = (z+zCell[k]) * settings->FCC_b;
                    for(int i=0;i<3;i++) {
                        positions[3*currentAtomIndex+i] = r[i];
                        initial_positions[3*currentAtomIndex+i] = r[i];
                        velocities[3*currentAtomIndex+i] = rnd->nextGauss()*sqrt(T*mass_inverse);
                    }

                    atom_type[currentAtomIndex] = ARGON;
                    atom_ids[currentAtomIndex] = currentAtomIndex;

                    currentAtomIndex++;
                }
            }
        }
    }
    countAtomTypes();
}

void System::init_parameters() {
    mass_inverse = 1.0/settings->mass;
    setRCut(settings->r_cut, false);
    one_over_r_cut_squared = 1.0/(m_rCut*m_rCut);
    m_dt = settings->dt;
    m_time = 0;

    setSystemSize(QVector3D(settings->unit_cells_x*settings->FCC_b,settings->unit_cells_x*settings->FCC_b,settings->unit_cells_x*settings->FCC_b), false);
    resetCells();
}

void System::set_topology() {
    /*----------------------------------------------------------------------
    Defines a logical network topology.  Prepares a neighbor-node ID table,
    nn, & a shift-vector table, sv, for internode message passing.  Also
    prepares the node parity table, myparity.
    ----------------------------------------------------------------------*/

    /* Integer vectors to specify the six neighbor nodes */
    int integer_vector[6][3] = {
        {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0}, {0,0,-1}, {0,0,1}
    };

    /* Set up neighbor tables, nn & sv */
    for (int n=0; n<6; n++) {
        /* Shift vector, sv */
        for (int a=0; a<3; a++) shift_vector[n][a] = m_systemSize[a]*integer_vector[n][a];
    }
}

inline void System::cell_index_from_ijk(const int &i, const int &j, const int &k, unsigned int &cell_index) {
    cell_index = i*num_cells_including_ghosts_yz+j*num_cells_including_ghosts[2]+k;
#ifdef MD_DEBUG
    if(cell_index > is_ghost_cell.size()-1 || cell_index < 0) {
        cout << systemSize().x() << endl;
        cout << "1: cell index: " << cell_index << endl;
        cout << "i: " << i << ", " << j << ", " << k << endl;
        exit(1);
    }
#endif
}

inline void System::cell_index_from_vector(unsigned int *mc, unsigned int &cell_index) {
    cell_index = mc[0]*num_cells_including_ghosts_yz+mc[1]*num_cells_including_ghosts[2]+mc[2];
#ifdef MD_DEBUG
    if(cell_index > is_ghost_cell.size()-1 || cell_index < 0) {
        cout << systemSize().x() << endl;
        cout << "2: cell index: " << cell_index << endl;
        cout << "i: " << mc[0] << ", " << mc[1] << ", " << mc[2] << endl;
        exit(1);
    }
#endif
}

inline bool System::atom_should_be_copied(atomDataType* ri, int ku) {
    int dimension,higher;
    dimension = ku/2; /* x(0)|y(1)|z(2) direction */
    higher = ku%2; /* Lower(0)|higher(1) direction */
    if (higher == 0) return ri[dimension] < m_rCut;
    else return ri[dimension] > m_systemSize[dimension]-m_rCut;
}


inline bool System::atom_did_change_node(atomDataType* ri, int ku) {
    int dimension,higher;
    dimension = ku/2;    /* x(0)|y(1)|z(2) direction */
    higher = ku%2; /* Lower(0)|higher(1) direction */
    if (higher == 0) return ri[dimension] < 0.0;
    else return ri[dimension] > m_systemSize[dimension];
}

void System::mpi_move() {
    int new_atoms = 0;
    int num_send,num_receive;
    short node_higher, node_lower, local_node_id;

    /* Reset the # of to-be-moved atoms, move_queue[][0] */
    for (short ku=0; ku<6; ku++) move_queue[ku][0] = 0;

    /* Make a moved-atom list, move_queue----------------------------------*/

    for(short dimension=0;dimension<3;dimension++) {
        /* Scan all the residents & immigrants to list moved-out atoms */
        for (unsigned long i=0; i<m_numAtoms+new_atoms; i++) {
            node_lower = 2*dimension;
            node_higher = 2*dimension+1;
            /* Register a to-be-copied atom in move_queue[kul|kuh][] */
            if (!atom_moved[i]) { /* Don't scan moved-out atoms */
                // Check if this atom moved
                if (atom_did_change_node(&positions[3*i],node_lower)) {
                    move_queue[node_lower][ ++move_queue[node_lower][0] ] = i;
                    count_periodic[dimension]--;
                }
                else if (atom_did_change_node(&positions[3*i],node_higher)) {
                    move_queue[node_higher][ ++move_queue[node_higher][0] ] = i;
                    count_periodic[dimension]++;
                }
            }
        }

        /* Message passing with neighbor nodes----------------------------*/

        /* Loop over the lower & higher directions------------------------*/
        for (int j=0; j<2; j++) {
            local_node_id=2*dimension+j;
            /* Send atom-number information---------------------------------*/

            num_send = move_queue[local_node_id][0]; /* # of atoms to-be-sent */

            num_receive = num_send;

            /* Message buffering */
            for (int i=1; i<=num_send; i++) {
                int atom_index = move_queue[local_node_id][i];
                /* Shift the coordinate origin */
                m_dataBuffer[11*(i-1)    + 0] = positions[ 3*atom_index + 0] - shift_vector[local_node_id][0];
                m_dataBuffer[11*(i-1)    + 1] = positions[ 3*atom_index + 1] - shift_vector[local_node_id][1];
                m_dataBuffer[11*(i-1)    + 2] = positions[ 3*atom_index + 2] - shift_vector[local_node_id][2];
                m_dataBuffer[11*(i-1)+ 3 + 0] = velocities[3*atom_index + 0];
                m_dataBuffer[11*(i-1)+ 3 + 1] = velocities[3*atom_index + 1];
                m_dataBuffer[11*(i-1)+ 3 + 2] = velocities[3*atom_index + 2];
                m_dataBuffer[11*(i-1)+ 6 + 0] = initial_positions[ 3*atom_index + 0] - shift_vector[local_node_id][0];
                m_dataBuffer[11*(i-1)+ 6 + 1] = initial_positions[ 3*atom_index + 1] - shift_vector[local_node_id][1];
                m_dataBuffer[11*(i-1)+ 6 + 2] = initial_positions[ 3*atom_index + 2] - shift_vector[local_node_id][2];
                m_dataBuffer[11*(i-1) + 9] = (float)atom_ids[atom_index];
                m_dataBuffer[11*(i-1) + 10] = (float)atom_type[atom_index];
                atom_moved[atom_index] = true;
            }

            /* Message storing */
            for (int i=0; i<num_receive; i++) {
                int atom_index = m_numAtoms+new_atoms+i;

                positions [3*atom_index+0] = m_dataBuffer[11*i   + 0];
                positions [3*atom_index+1] = m_dataBuffer[11*i   + 1];
                positions [3*atom_index+2] = m_dataBuffer[11*i   + 2];

                velocities[3*atom_index + 0] = m_dataBuffer[11*i+3 + 0];
                velocities[3*atom_index + 1] = m_dataBuffer[11*i+3 + 1];
                velocities[3*atom_index + 2] = m_dataBuffer[11*i+3 + 2];

                initial_positions [3*atom_index+0] = m_dataBuffer[11*i+6  + 0];
                initial_positions [3*atom_index+1] = m_dataBuffer[11*i+6  + 1];
                initial_positions [3*atom_index+2] = m_dataBuffer[11*i+6  + 2];

                atom_ids[atom_index] = (unsigned long) m_dataBuffer[11*i + 9];
                atom_type[atom_index] = (unsigned long) m_dataBuffer[11*i + 10];
                atom_moved[atom_index] = false;
            }
            /* Increment the # of new atoms */
            new_atoms += num_receive;

        } /* Endfor lower & higher directions, j */
    }

    int ipt = 0;
    for (int i=0; i<int(m_numAtoms+new_atoms); i++) {
        if (!atom_moved[i]) {
            for (int a=0; a<3; a++) {
                positions [3*ipt+a] = positions [3*i+a];
                initial_positions [3*ipt+a] = initial_positions [3*i+a];
                velocities[3*ipt+a] = velocities[3*i+a];
            }
            atom_type[ipt] = atom_type[i];
            atom_ids[ipt] = atom_ids[i];

            ipt++;
        }
    }

    /* Update the compressed # of resident atoms */
    m_numAtoms = ipt;
//    num_atoms_free = m_numAtoms - num_atoms_free;
}

void System::mpi_copy() {
    int num_send, num_receive;
    int new_ghost_atoms = 0;
    short local_node_id;
    for(short dimension=0;dimension<3;dimension++) {
        for (int higher=0; higher<2; higher++) move_queue[2*dimension+higher][0] = 0;
        for(int i=0;i<int(m_numAtoms+new_ghost_atoms);i++) {
            for(int higher=0;higher<2;higher++) {
                local_node_id = 2*dimension + higher;
                if (atom_should_be_copied(&positions[3*i],local_node_id)) move_queue[local_node_id][++(move_queue[local_node_id][0])] = i;
            }
        }

        /* Loop through higher and lower node in this dimension */
        for(int higher=0;higher<2;higher++) {
            local_node_id= 2*dimension+higher;
            num_send = move_queue[local_node_id][0];

            num_receive = num_send;

            for (int i=1; i<=num_send; i++) {
                int atom_index = move_queue[local_node_id][i];
                /* Shift the coordinate origin */
                m_dataBuffer[3*(i-1)+0] = positions[ 3*atom_index + 0]-shift_vector[local_node_id][0];
                m_dataBuffer[3*(i-1)+1] = positions[ 3*atom_index + 1]-shift_vector[local_node_id][1];
                m_dataBuffer[3*(i-1)+2] = positions[ 3*atom_index + 2]-shift_vector[local_node_id][2];
            }

            for (int i=0; i<num_receive; i++) {
                positions[ 3*(m_numAtoms+new_ghost_atoms+i) + 0] = m_dataBuffer[3*i+0];
                positions[ 3*(m_numAtoms+new_ghost_atoms+i) + 1] = m_dataBuffer[3*i+1];
                positions[ 3*(m_numAtoms+new_ghost_atoms+i) + 2] = m_dataBuffer[3*i+2];
            }

            new_ghost_atoms += num_receive;
        }
    }

    num_atoms_ghost = new_ghost_atoms;
}

void System::half_kick() {
    double maxMagnitude = 1.0;
    double density = m_numAtoms / volume();
    double volumePerAtom = 1.0/density;
    double lengthPerAtom = pow(volumePerAtom, 1.0/3);

    double minimumSystemSizeComponentSquared = 0.1*lengthPerAtom; // will square next line
    minimumSystemSizeComponentSquared *= minimumSystemSizeComponentSquared;
    double maxVelocitySquared = minimumSystemSizeComponentSquared/(m_dt*m_dt);

    for(int n=0;n<int(m_numAtoms);n++) {
        if(atom_type[n] == FIXED) {
            continue;
        }

        double magnitude = sqrt(accelerations[3*n+0]*m_dt*0.5*accelerations[3*n+0]*m_dt*0.5 + accelerations[3*n+1]*m_dt*0.5*accelerations[3*n+1]*m_dt*0.5 + accelerations[3*n+2]*m_dt*0.5*accelerations[3*n+2]*m_dt*0.5);
        double scaling = (magnitude>maxMagnitude) ? maxMagnitude/magnitude : 1.0;

        velocities[3*n+0] += accelerations[3*n+0]*m_dt*0.5*scaling;
        velocities[3*n+1] += accelerations[3*n+1]*m_dt*0.5*scaling;
        velocities[3*n+2] += accelerations[3*n+2]*m_dt*0.5*scaling;

        double velocitySquared = velocities[3*n+0]*velocities[3*n+0] + velocities[3*n+1]*velocities[3*n+1] + velocities[3*n+2]*velocities[3*n+2];
        if(velocitySquared>maxVelocitySquared) {
            m_didScaleVelocitiesDueToHighValues = true;
            double scaling2 = sqrt(maxVelocitySquared/velocitySquared);
            velocities[3*n+0] *= scaling2;
            velocities[3*n+1] *= scaling2;
            velocities[3*n+2] *= scaling2;
        }

        magnitude = sqrt(accelerations[3*n+0]*m_dt*0.5*accelerations[3*n+0]*m_dt*0.5 + accelerations[3*n+1]*m_dt*0.5*accelerations[3*n+1]*m_dt*0.5 + accelerations[3*n+2]*m_dt*0.5*accelerations[3*n+2]*m_dt*0.5)*scaling;
    }
}

void System::full_kick() {
    for(int n=0;n<int(m_numAtoms);n++) {
        if(atom_type[n] == FIXED) {
            continue;
        }
        velocities[3*n+0] += accelerations[3*n+0]*m_dt;
        velocities[3*n+1] += accelerations[3*n+1]*m_dt;
        velocities[3*n+2] += accelerations[3*n+2]*m_dt;
    }
}

void System::move() {
    for(int n=0;n<int(m_numAtoms);n++) {
        atom_moved[n] = false;

        if(atom_type[n] == FIXED) {
            continue;
        }
        positions[3*n+0] += velocities[3*n+0]*m_dt;
        positions[3*n+1] += velocities[3*n+1]*m_dt;
        positions[3*n+2] += velocities[3*n+2]*m_dt;
    }
}

void System::apply_gravity() {
    double gravity_force_times_dt = settings->gravity_force*m_dt;
    for(int n=0;n<int(m_numAtoms);n++) {
        if(atom_type[n] == FIXED) {
            continue;
        }
        if(atom_type[n] == FROZEN) {
            continue;
        }
        velocities[3*n+settings->gravity_direction] += gravity_force_times_dt;
    }
}

void System::apply_harmonic_oscillator() {
    double spring_constant = 2000.0;
    double spring_constant_times_mass_inverse = spring_constant * mass_inverse;
    for(int n=0; n<int(m_numAtoms); n++) {
        if(atom_type[n] == FROZEN) {
            double dx = positions[3*n+0] - initial_positions[3*n+0];
            double dy = positions[3*n+1] - initial_positions[3*n+1];
            double dz = positions[3*n+2] - initial_positions[3*n+2];
            double dr2 = dx*dx + dy*dy + dz*dz;

            accelerations[3*n+0] += -spring_constant_times_mass_inverse*dx;
            accelerations[3*n+1] += -spring_constant_times_mass_inverse*dy;
            accelerations[3*n+2] += -spring_constant_times_mass_inverse*dz;
            m_potentialEnergy += 0.5*spring_constant*dr2;
        }
    }
}

void System::reset() {
    /* Reset the potential, pressure & forces */
//    memset(&accelerations[0],0,3*m_numAtoms*sizeof(double));
    fill(accelerations.begin(), accelerations.begin() + 3*m_numAtoms, 0);
    m_potentialEnergy = 0;
    pressure_forces = 0;
}

void System::ensureAllAtomsAreInsideSystem() {
    for(int n=0;n<int(m_numAtoms);n++) {
        for(int a = 0; a < 3; a++) {
            if(positions[3*n+a] < 0 || positions[3*n+a] >= m_systemSize[a]) {
                double shift = int(positions[3*n+a] / m_systemSize[a]) * m_systemSize[a];
                positions[3*n+a] -= shift;
                initial_positions[3*n+a] -= shift;
            }
        }
    }
}

void System::step() {
    m_didScaleVelocitiesDueToHighValues = false;
    half_kick();
    move();
    mpi_move();
    mpi_copy();
    reset();
    if(settings->gravity_direction >= 0) {
        apply_gravity();
    }
    calculate_accelerations();
    apply_harmonic_oscillator();
    half_kick();
//    ensureAllAtomsAreInsideSystem();

    steps++;
    m_time += m_dt;
}
