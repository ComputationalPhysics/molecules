#pragma once

class Atom;
class ThreadControl;
class Settings;
class MDIO;
class MDTimer;
class Random;
class UnitConverter;

#include <fstream>
#include <vector>
#include <vec3.h>

#define EMPTY -1

typedef double atomDataType;

using namespace std;

class System {
private:
    void allocate();
    void move();
    void mpi_move();
    void mpi_copy();
    void calculate_accelerations();
    void calculate_accelerations_many_frozen_atoms();
    void apply_gravity();
    void full_kick();
    void half_kick();

    void set_topology();
    void init_parameters();
    void create_FCC();
    void reset();
    inline bool atom_did_change_node(atomDataType* ri, int ku);
    inline bool atom_should_be_copied(atomDataType *ri, int ku);
    inline void cell_index_from_ijk(const int &i, const int &j, const int &k, unsigned int &cell_index);
    inline void cell_index_from_vector(unsigned int *mc, unsigned int &cell_index);
    void createForcesAndPotentialTable();

    vec3 m_systemSize;
    vec3 cellLength;

public:
    Settings *settings;
    MDIO *mdio;
    Random *rnd;
//    MDTimer *mdtimer;
    UnitConverter *unit_converter;
    int  *head_all_atoms;
    int  *head_free_atoms;

    bool sample_statistics;
    unsigned long steps;
    int neighbor_nodes[6];
    int max_number_of_atoms;
    int max_number_of_cells;
    unsigned long num_atoms;
    unsigned long num_atoms_free;
    unsigned long num_atoms_frozen;
    unsigned long num_atoms_ghost;


    long i,j,k,n,m,a,b,c, nx, ny, nz;
    long count_periodic[3];

    double r_cut, dt, dt_half, potential_energy, t, t0, volume, one_over_r_cut_squared;
    unsigned int mc[3];  // Usually cell index vector
    unsigned int mc1[3]; // Usually cell index vector
    unsigned int num_cells_including_ghosts_yz,cell_index, cell_index_2,num_cells_including_ghosts_xyz;
    unsigned int num_cells[3];
    unsigned int num_cells_including_ghosts[3];
    double dr[3];
    double shift_vector[6][3];
    unsigned int **move_queue;

    vector<atomDataType> mpi_send_buffer;
    vector<atomDataType> mpi_receive_buffer;
    vector<bool> atom_moved;
    vector<unsigned long> atom_ids;
    vector<atomDataType> positions;
    vector<atomDataType> velocities;
    vector<atomDataType> accelerations;

    int numberOfPrecomputedTwoParticleForces;
    double deltaR2;
    double oneOverDeltaR2;
    vector<double> precomputed_forces;
    vector<double> precomputed_potential;

    double mass_inverse, pressure_forces;

    vector<unsigned long> atom_type;
    int *linked_list_all_atoms;
    int *linked_list_free_atoms;
    bool *is_ghost_cell;
    vector<float> initial_positions;
    void apply_harmonic_oscillator();
    void count_frozen_atoms();

    System();
    void setup(Settings *settings_);
    void step();
    double get_volume() {
        return m_systemSize[0]*m_systemSize[1]*m_systemSize[2];
    }
    vec3 systemSize() const;
    void setSystemSize(const vec3 &systemSize);
};
