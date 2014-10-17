#include <iostream>
#include <math.h>
#include <time.h>
#include <fstream>
#include <system.h>
#include <settings.h>
#include <unitconverter.h>
#include <random.h>
#include <statisticssampler.h>
#include <atom_types.h>
#include <string.h>

#include <potential_lennard_jones.h>

using namespace std;
System::System() :
    positions(NULL),
    accelerations(NULL),
    velocities(NULL),
    atom_type(NULL),
    atom_moved(NULL),
    mpi_send_buffer(NULL),
    mpi_receive_buffer(NULL),
    atom_ids(NULL),
    linked_list_all_atoms(NULL),
    linked_list_free_atoms(NULL),
    is_ghost_cell(NULL),
    initial_positions(NULL),
    move_queue(NULL),
    t(0)
{

}

void System::allocate() {
    positions = new double[3*max_number_of_atoms];
    accelerations = new double[3*max_number_of_atoms];
    velocities = new double[3*max_number_of_atoms];
    atom_type = new unsigned long[max_number_of_atoms];
    atom_moved = new bool[max_number_of_atoms];
    mpi_send_buffer = new double[max_number_of_atoms];
    mpi_receive_buffer = new double[max_number_of_atoms];
    atom_ids = new unsigned long[max_number_of_atoms];

    linked_list_all_atoms = new int[max_number_of_atoms];
    linked_list_free_atoms = new int[max_number_of_atoms];
    is_ghost_cell = new bool[max_number_of_cells];
    initial_positions = new double[3*max_number_of_atoms];
    move_queue = new unsigned int*[6];
    for(int i=0; i<6; i++) {
        move_queue[i] = new unsigned int[max_number_of_atoms];
    }

    memset(positions, 0, 3*max_number_of_atoms*sizeof(double));
    memset(accelerations, 0, 3*max_number_of_atoms*sizeof(double));
    memset(velocities, 0, 3*max_number_of_atoms*sizeof(double));
    memset(atom_type, 0, max_number_of_atoms*sizeof(unsigned long));
    memset(atom_moved, 0, max_number_of_atoms*sizeof(bool));
    memset(mpi_receive_buffer, 0, max_number_of_atoms*sizeof(double));
    memset(mpi_send_buffer, 0, max_number_of_atoms*sizeof(double));
    memset(atom_ids, 0, max_number_of_atoms*sizeof(unsigned long));
    memset(linked_list_free_atoms, 0, max_number_of_atoms*sizeof(int));
    memset(linked_list_all_atoms, 0, max_number_of_atoms*sizeof(int));
    memset(is_ghost_cell, 0, max_number_of_cells*sizeof(bool));
    memset(initial_positions, 0, 3*max_number_of_atoms*sizeof(double));
}

void System::setup(int myid_, Settings *settings_) {
    unit_converter = new UnitConverter();

    myid = myid_;
    settings = settings_;
    num_atoms = 0;
    num_atoms_ghost = 0;
    num_nodes = settings->nodes_x*settings->nodes_y*settings->nodes_z;
    max_number_of_atoms = settings->max_number_of_atoms;
    max_number_of_cells = settings->max_number_of_cells;

    allocate();

    steps = 0;
    int seed = -(myid+1);
    rnd = new Random(seed);

    init_parameters();

    set_topology();
    create_FCC();

    count_frozen_atoms();

    mpi_copy();
    calculate_accelerations();
    half_kick();

    if(myid==0) cout << "System size: " << unit_converter->length_to_SI(system_length[0])*1e10 << " Å " << unit_converter->length_to_SI(system_length[1])*1e10 << " Å " << unit_converter->length_to_SI(system_length[2])*1e10 << " Å" << endl;
    if(myid==0) cout << "Atoms: " << num_atoms << endl;
    if(myid==0) cout << "Free atoms: " << num_atoms_free << endl;
    if(myid==0) cout << "Processors: " << num_nodes << " (" << settings->nodes_x << "," << settings->nodes_y << "," << settings->nodes_z << ")" << endl;
    if(myid==0) cout << "Free atoms/processor: " << num_atoms_free/num_nodes << endl;
}

void System::count_frozen_atoms() {
    num_atoms_free = 0;
    num_atoms_frozen = 0;
    for(int i=0; i<num_atoms; i++) {
        if(atom_type[i] == FROZEN)  {
            num_atoms_frozen++;
        } else {
            num_atoms_free++;
        }
    }
}

void System::create_FCC() {
    double xCell[4] = {0, 0.5, 0.5, 0};
    double yCell[4] = {0, 0.5, 0, 0.5};
    double zCell[4] = {0, 0, 0.5, 0.5};

    double r[3];
    double T = settings->temperature;

    bool warning_shown = false;
    for(int x = 0; x < settings->unit_cells_x; x++) {
        for(int y = 0; y < settings->unit_cells_y; y++) {
            for(int z = 0; z < settings->unit_cells_z; z++) {
                for(int k = 0; k < 4; k++) {
                    // Set positions and type

                    r[0] = (x+xCell[k]) * settings->FCC_b;
                    r[1] = (y+yCell[k]) * settings->FCC_b;
                    r[2] = (z+zCell[k]) * settings->FCC_b;
                    for(i=0;i<3;i++) {
                        positions[3*num_atoms+i] = r[i];
                        initial_positions[3*num_atoms+i] = r[i];
                        velocities[3*num_atoms+i] = rnd->nextGauss()*sqrt(T*mass_inverse);
                    }

                    atom_type[num_atoms] = ARGON;
                    atom_ids[num_atoms] = myid*max_number_of_atoms + num_atoms;

                    num_atoms++;
                    if(!warning_shown && num_atoms >= 0.8*max_number_of_atoms) {
                        cout << "                 ### WARNING ###" << endl;
                        cout << "NUMBER OF PARTICLES IS MORE THAN 0.8*MAX_ATOM_NUM" << endl << endl;
                        warning_shown = true;
                    }
                }
            }
        }
    }
}

void System::init_parameters() {
    mass_inverse = 1.0/settings->mass;
    r_cut = settings->r_cut;
    dt = settings->dt;
    dt_half = dt/2;
    t = 0;

    num_processors[0] = settings->nodes_x;
    num_processors[1] = settings->nodes_y;
    num_processors[2] = settings->nodes_z;

    node_index[0] = myid/(settings->nodes_y*settings->nodes_z);
    node_index[1] = (myid/settings->nodes_z) % settings->nodes_y;
    node_index[2] = myid%settings->nodes_z;

    // Size of this node
    node_length[0] = settings->unit_cells_x*settings->FCC_b;
    node_length[1] = settings->unit_cells_y*settings->FCC_b;
    node_length[2] = settings->unit_cells_z*settings->FCC_b;

    for(a=0;a<3;a++) {
        system_length[a] = node_length[a]*num_processors[a];
        origo[a] = (double)node_index[a] * node_length[a];
        num_cells_local[a] = node_length[a]/r_cut;
        num_cells_including_ghosts[a] = num_cells_local[a]+2;

        cell_length[a] = node_length[a]/num_cells_local[a];
        count_periodic[a] = 0;
    }
    volume = system_length[0]*system_length[1]*system_length[2];

    num_cells_including_ghosts_yz = num_cells_including_ghosts[1]*num_cells_including_ghosts[2];
    num_cells_including_ghosts_xyz = num_cells_including_ghosts_yz*num_cells_including_ghosts[0];
    head_all_atoms = new int[num_cells_including_ghosts_xyz];
    head_free_atoms = new int[num_cells_including_ghosts_xyz];

    for(int cx=0;cx<num_cells_local[0]+2;cx++) {
        for(int cy=0;cy<num_cells_local[1]+2;cy++) {
            for(int cz=0;cz<num_cells_local[2]+2;cz++) {
                cell_index_from_ijk(cx,cy,cz,cell_index);
                if(cx == 0 || cx == num_cells_local[0]+1 || cy == 0 || cy == num_cells_local[1]+1 || cz == 0 || cz == num_cells_local[2]+1) {
                    is_ghost_cell[cell_index] = true;
                } else is_ghost_cell[cell_index] = false;
            }
        }
    }
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

    int k1[3];

    /* Set up neighbor tables, nn & sv */
    for (int n=0; n<6; n++) {
        /* Vector index of neighbor ku */
        for (a=0; a<3; a++) {
            k1[a] = (node_index[a]+integer_vector[n][a]+num_processors[a])%num_processors[a];
        }

        /* Scalar neighbor ID, nn */
        neighbor_nodes[n] = k1[0]*num_processors[1]*num_processors[2]+k1[1]*num_processors[2]+k1[2];
        /* Shift vector, sv */
        for (a=0; a<3; a++) shift_vector[n][a] = node_length[a]*integer_vector[n][a];
    }



    /* Set up the node parity table, myparity */
    for (a=0; a<3; a++) {
        if (num_processors[a] == 1) {
            my_parity[a] = 2;
        } else if (node_index[a]%2 == 0) {
            my_parity[a] = 0;
        } else {
            my_parity[a] = 1;
        }
    }
}

inline void System::cell_index_from_ijk(const int &i, const int &j, const int &k, unsigned int &cell_index) {
    cell_index = i*num_cells_including_ghosts_yz+j*num_cells_including_ghosts[2]+k;
#ifdef MD_DEBUG
    if(cell_index > max_number_of_cells-1) {
        cout << "Too few cells, aborting. Increase program.max_number_of_cells in python script" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1000);
    }
#endif
}

inline void System::cell_index_from_vector(unsigned int *mc, unsigned int &cell_index) {
    cell_index = mc[0]*num_cells_including_ghosts_yz+mc[1]*num_cells_including_ghosts[2]+mc[2];
#ifdef MD_DEBUG
    if(cell_index > max_number_of_cells-1) {
        cout << "Too few cells, aborting. Increase program.max_number_of_cells in python script" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1000);
    }
#endif
}

inline bool System::atom_should_be_copied(double* ri, int ku) {
    int dimension,higher;
    dimension = ku/2; /* x(0)|y(1)|z(2) direction */
    higher = ku%2; /* Lower(0)|higher(1) direction */
    if (higher == 0) return ri[dimension] < r_cut;
    else return ri[dimension] > node_length[dimension]-r_cut;
}


inline bool System::atom_did_change_node(double* ri, int ku) {
    int dimension,higher;
    dimension = ku/2;    /* x(0)|y(1)|z(2) direction */
    higher = ku%2; /* Lower(0)|higher(1) direction */
    if (higher == 0) return ri[dimension] < 0.0;
    else return ri[dimension] > node_length[dimension];
}

void System::mpi_move() {
    int new_atoms = 0;
    int node_id,num_send,num_receive;
    short node_higher, node_lower, local_node_id;

    /* Reset the # of to-be-moved atoms, move_queue[][0] */
    for (short ku=0; ku<6; ku++) move_queue[ku][0] = 0;

    /* Make a moved-atom list, move_queue----------------------------------*/

    for(short dimension=0;dimension<3;dimension++) {
        /* Scan all the residents & immigrants to list moved-out atoms */
        for (i=0; i<num_atoms+new_atoms; i++) {
            node_lower = 2*dimension;
            node_higher = 2*dimension+1;
            /* Register a to-be-copied atom in move_queue[kul|kuh][] */
            if (!atom_moved[i]) { /* Don't scan moved-out atoms */
                // Check if this atom moved
                if (atom_did_change_node(&positions[3*i],node_lower)) {
                    move_queue[node_lower][ ++move_queue[node_lower][0] ] = i;
                    if(node_index[dimension] == 0) count_periodic[dimension]--;
                }
                else if (atom_did_change_node(&positions[3*i],node_higher)) {
                    move_queue[node_higher][ ++move_queue[node_higher][0] ] = i;
                    if(node_index[dimension] == num_processors[dimension]-1) count_periodic[dimension]++;
                }
            }
        }

        /* Message passing with neighbor nodes----------------------------*/

        /* Loop over the lower & higher directions------------------------*/
        for (j=0; j<2; j++) {
            local_node_id=2*dimension+j;
            node_id = neighbor_nodes[local_node_id]; /* Neighbor node ID */
            /* Send atom-number information---------------------------------*/

            num_send = move_queue[local_node_id][0]; /* # of atoms to-be-sent */

            num_receive = num_send;

            /* Message buffering */
            for (i=1; i<=num_send; i++) {
                int atom_index = move_queue[local_node_id][i];
                /* Shift the coordinate origin */
                mpi_send_buffer[11*(i-1)    + 0] = positions[ 3*atom_index + 0] - shift_vector[local_node_id][0];
                mpi_send_buffer[11*(i-1)    + 1] = positions[ 3*atom_index + 1] - shift_vector[local_node_id][1];
                mpi_send_buffer[11*(i-1)    + 2] = positions[ 3*atom_index + 2] - shift_vector[local_node_id][2];
                mpi_send_buffer[11*(i-1)+ 3 + 0] = velocities[3*atom_index + 0];
                mpi_send_buffer[11*(i-1)+ 3 + 1] = velocities[3*atom_index + 1];
                mpi_send_buffer[11*(i-1)+ 3 + 2] = velocities[3*atom_index + 2];
                mpi_send_buffer[11*(i-1)+ 6 + 0] = initial_positions[ 3*atom_index + 0] - shift_vector[local_node_id][0];
                mpi_send_buffer[11*(i-1)+ 6 + 1] = initial_positions[ 3*atom_index + 1] - shift_vector[local_node_id][1];
                mpi_send_buffer[11*(i-1)+ 6 + 2] = initial_positions[ 3*atom_index + 2] - shift_vector[local_node_id][2];
                mpi_send_buffer[11*(i-1) + 9] = (double)atom_ids[atom_index];
                mpi_send_buffer[11*(i-1) + 10] = (double)atom_type[atom_index];
                atom_moved[atom_index] = true;
            }

            memcpy(mpi_receive_buffer,mpi_send_buffer,11*num_receive*sizeof(double));

            /* Message storing */
            for (i=0; i<num_receive; i++) {
                int atom_index = num_atoms+new_atoms+i;

                positions [3*atom_index+0] = mpi_receive_buffer[11*i   + 0];
                positions [3*atom_index+1] = mpi_receive_buffer[11*i   + 1];
                positions [3*atom_index+2] = mpi_receive_buffer[11*i   + 2];

                velocities[3*atom_index + 0] = mpi_receive_buffer[11*i+3 + 0];
                velocities[3*atom_index + 1] = mpi_receive_buffer[11*i+3 + 1];
                velocities[3*atom_index + 2] = mpi_receive_buffer[11*i+3 + 2];

                initial_positions [3*atom_index+0] = mpi_receive_buffer[11*i+6  + 0];
                initial_positions [3*atom_index+1] = mpi_receive_buffer[11*i+6  + 1];
                initial_positions [3*atom_index+2] = mpi_receive_buffer[11*i+6  + 2];

                atom_ids[atom_index] = (unsigned long) mpi_receive_buffer[11*i + 9];
                atom_type[atom_index] = (unsigned long) mpi_receive_buffer[11*i + 10];
                atom_moved[atom_index] = false;
            }
            /* Increment the # of new atoms */
            new_atoms += num_receive;

        } /* Endfor lower & higher directions, j */
    }

    int ipt = 0;
    for (i=0; i<num_atoms+new_atoms; i++) {
        if (!atom_moved[i]) {
            for (a=0; a<3; a++) {
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
    num_atoms = ipt;
    num_atoms_free = num_atoms - num_atoms_free;
}

void System::mpi_copy() {
    int node_id, num_send, num_receive;
    int new_ghost_atoms = 0;
    short higher, local_node_id;
    for(short dimension=0;dimension<3;dimension++) {
        for (higher=0; higher<2; higher++) move_queue[2*dimension+higher][0] = 0;
        for(i=0;i<num_atoms+new_ghost_atoms;i++) {
            for(higher=0;higher<2;higher++) {
                local_node_id = 2*dimension + higher;
                if (atom_should_be_copied(&positions[3*i],local_node_id)) move_queue[local_node_id][++(move_queue[local_node_id][0])] = i;
            }
        }

        /* Loop through higher and lower node in this dimension */
        for(higher=0;higher<2;higher++) {
            local_node_id= 2*dimension+higher;
            node_id = neighbor_nodes[local_node_id];
            num_send = move_queue[local_node_id][0];

            num_receive = num_send;

            for (i=1; i<=num_send; i++) {
                int atom_index = move_queue[local_node_id][i];
                /* Shift the coordinate origin */
                mpi_send_buffer[3*(i-1)+0] = positions[ 3*atom_index + 0]-shift_vector[local_node_id][0];
                mpi_send_buffer[3*(i-1)+1] = positions[ 3*atom_index + 1]-shift_vector[local_node_id][1];
                mpi_send_buffer[3*(i-1)+2] = positions[ 3*atom_index + 2]-shift_vector[local_node_id][2];
            }

            memcpy(mpi_receive_buffer,mpi_send_buffer,3*num_receive*sizeof(double));

            for (i=0; i<num_receive; i++) {
                positions[ 3*(num_atoms+new_ghost_atoms+i) + 0] = mpi_receive_buffer[3*i+0];
                positions[ 3*(num_atoms+new_ghost_atoms+i) + 1] = mpi_receive_buffer[3*i+1];
                positions[ 3*(num_atoms+new_ghost_atoms+i) + 2] = mpi_receive_buffer[3*i+2];
            }

            new_ghost_atoms += num_receive;
        }
    }

    num_atoms_ghost = new_ghost_atoms;
}

void System::half_kick() {
    for(n=0;n<num_atoms;n++) {
        velocities[3*n+0] += accelerations[3*n+0]*dt_half;
        velocities[3*n+1] += accelerations[3*n+1]*dt_half;
        velocities[3*n+2] += accelerations[3*n+2]*dt_half;
    }
}

void System::full_kick() {
    for(n=0;n<num_atoms;n++) {
        velocities[3*n+0] += accelerations[3*n+0]*dt;
        velocities[3*n+1] += accelerations[3*n+1]*dt;
        velocities[3*n+2] += accelerations[3*n+2]*dt;
    }
}

void System::move() {
    for(n=0;n<num_atoms;n++) {
        positions[3*n+0] += velocities[3*n+0]*dt;
        positions[3*n+1] += velocities[3*n+1]*dt;
        positions[3*n+2] += velocities[3*n+2]*dt;

        atom_moved[n] = false;
    }
}

void System::apply_gravity() {
    double gravity_force_times_dt = settings->gravity_force*dt;
    for(n=0;n<num_atoms;n++) {
        if(atom_type[n] != FROZEN) velocities[3*n+settings->gravity_direction] += gravity_force_times_dt;
    }
}

void System::apply_harmonic_oscillator() {
    double spring_constant = 1000.0;
    double spring_constant_times_mass_inverse = spring_constant * mass_inverse;
    for(n=0; n<num_atoms; n++) {
        if(atom_type[n] == FROZEN) {
            double dx = positions[3*n+0] - initial_positions[3*n+0];
            double dy = positions[3*n+1] - initial_positions[3*n+1];
            double dz = positions[3*n+2] - initial_positions[3*n+2];
            double dr2 = dx*dx + dy*dy + dz*dz;

            accelerations[3*n+0] += -spring_constant_times_mass_inverse*dx;
            accelerations[3*n+1] += -spring_constant_times_mass_inverse*dy;
            accelerations[3*n+2] += -spring_constant_times_mass_inverse*dz;
            potential_energy += 0.5*spring_constant*dr2;
        }
    }
}

void System::reset() {
    /* Reset the potential, pressure & forces */
    memset(accelerations,0,3*num_atoms*sizeof(double));
    potential_energy = 0;
    pressure_forces = 0;
}

void System::step() {
    half_kick();
    move();
    mpi_move();
    mpi_copy();
    reset();
    if(settings->gravity_direction >= 0) apply_gravity();
    calculate_accelerations();
    apply_harmonic_oscillator();
    half_kick();

    steps++;
    t += dt;
}
