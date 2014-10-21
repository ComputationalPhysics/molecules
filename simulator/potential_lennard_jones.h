#include <system.h>
#include <settings.h>

// #DEFINE PRECOMPUTED_TABLE

/* Calculates the lennard jones potential between all free atoms within dr<=r_cut.
 * Skips all frozen atom pairs.
 * Calculates potential energy and virial term for pressure calculation.
 *
 */
void System::calculate_accelerations() {
    double rr_cut = m_rCut*m_rCut;

    for (unsigned int c=0; c<num_cells_including_ghosts_xyz; c++) { head_all_atoms[c] = EMPTY; head_free_atoms[c] = EMPTY; }

    for (unsigned int i=0; i<m_numAtoms+num_atoms_ghost; i++) {
        for (int a=0; a<3; a++) mc[a] = (positions[3*i+a]+cellLength[a])/cellLength[a];
        if(mc[0] > 100000) {
            if(i>= m_numAtoms) {
                cout << "Atom " << i << " has x position " << positions[3*i+0] << " and is outside the system with cell length: " << cellLength[0] << endl;
            } else {
                cout << "Ghost atom " << i << " has x position " << positions[3*i+0] << " and is outside the system with cell length: " << cellLength[0] << endl;
            }
        }

        if(mc[1] > 100000) {
            if(i>= m_numAtoms) {
                cout << "Atom " << i << " has y position " << positions[3*i+1] << " and is outside the system with cell length: " << cellLength[1] << endl;
            } else {
                cout << "Ghost atom " << i << " has y position " << positions[3*i+1] << " and is outside the system with cell length: " << cellLength[1] << endl;
            }
        }

        if(mc[2] > 100000) {
            if(i>= m_numAtoms) {
                cout << "Atom " << i << " has z position " << positions[3*i+2] << " and is outside the system with cell length: " << cellLength[2] << endl;
            } else {
                cout << "Ghost atom " << i << " has z position " << positions[3*i+2] << " and is outside the system with cell length: " << cellLength[2] << endl;
            }
        }
        cell_index_from_vector(mc,cell_index);

        // Set this atom at the head of the linked list, and update its next
        linked_list_all_atoms[i] = head_all_atoms[cell_index];
        head_all_atoms[cell_index] = i;
    }

    atomDataType mass_inverse_24 = mass_inverse*24;

    atomDataType r_cut_squared_inverse = 1.0/rr_cut;
    atomDataType r_cut_6_inverse = r_cut_squared_inverse*r_cut_squared_inverse*r_cut_squared_inverse;
    atomDataType potential_energy_correction = 4*r_cut_6_inverse*(r_cut_6_inverse - 1);
    float dr[3];
    // Loop through all local cells (not including ghosts)
    for (mc[0]=1; mc[0]<=num_cells[0]; mc[0]++) {
        for (mc[1]=1; mc[1]<=num_cells[1]; mc[1]++) {
            for (mc[2]=1; mc[2]<=num_cells[2]; mc[2]++) {
                cell_index = mc[0]*num_cells_including_ghosts_yz+mc[1]*num_cells_including_ghosts[2]+mc[2];
                if ( head_all_atoms[cell_index] == EMPTY ) continue;

                // Loop through all neighbors (including ghosts) of this cell.
                for (mc1[0]=mc[0]-1; mc1[0]<=mc[0]+1; mc1[0]++) {
                    for (mc1[1]=mc[1]-1; mc1[1]<=mc[1]+1; mc1[1]++) {
                        for (mc1[2]=mc[2]-1; mc1[2]<=mc[2]+1; mc1[2]++) {
                            cell_index_2 = mc1[0]*num_cells_including_ghosts_yz+mc1[1]*num_cells_including_ghosts[2]+mc1[2];

                            if(head_all_atoms[cell_index_2] == EMPTY) continue;
                            unsigned long i = head_all_atoms[cell_index]; // Index of local atom i

                            while (i != EMPTY) {
                                unsigned long j = head_all_atoms[cell_index_2]; // Index of atom j
                                while (j != EMPTY) {
                                    if(i < j) { // Newton's 3rd law
                                        dr[0] = positions[3*i+0]-positions[3*j+0];
                                        dr[1] = positions[3*i+1]-positions[3*j+1];
                                        dr[2] = positions[3*i+2]-positions[3*j+2];
                                        atomDataType dr2 = dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2];

                                        if (dr2<rr_cut) {
#ifdef PRECOMPUTED_TABLE
                                            int precomputedTableIndex = dr2*one_over_r_cut_squared*numberOfPrecomputedTwoParticleForces;
                                            double force0 = precomputed_forces[precomputedTableIndex];
                                            double force1 = precomputed_forces[precomputedTableIndex+1];
                                            // Linearly interpolate between these values
                                            double force = force0 + (force1 - force0)*(dr2 - precomputedTableIndex*deltaR2)*oneOverDeltaR2;
#else

                                            atomDataType dr2_inverse = 1.0/dr2;
                                            atomDataType dr6_inverse = dr2_inverse*dr2_inverse*dr2_inverse;

                                            atomDataType force = (2*dr6_inverse-1)*dr6_inverse*dr2_inverse*mass_inverse_24;
#endif

                                            bool is_local_atom = j < m_numAtoms; // Ghost atoms contributes with 0.5 of pressure and potential energy statistics
                                            if(sample_statistics) {
#ifdef PRECOMPUTED_TABLE
                                                double energy0 = precomputed_potential[precomputedTableIndex];
                                                double energy1 = precomputed_potential[precomputedTableIndex+1];
                                                double potential_energy_tmp = energy0 + (energy1 - energy0)*(dr2 - precomputedTableIndex*deltaR2)*oneOverDeltaR2;
#else
                                                atomDataType potential_energy_tmp = 4*dr6_inverse*(dr6_inverse - 1) - potential_energy_correction;
#endif
                                                if(is_local_atom) {
                                                    m_potentialEnergy += potential_energy_tmp;
                                                    pressure_forces += force*dr2;
                                                } else {
                                                    pressure_forces += 0.5*force*dr2;
                                                    m_potentialEnergy += 0.5*potential_energy_tmp;
                                                }
                                            }

                                            accelerations[3*i+0] += force*dr[0];
                                            accelerations[3*i+1] += force*dr[1];
                                            accelerations[3*i+2] += force*dr[2];

                                            if(is_local_atom) {
                                                accelerations[3*j+0] -= force*dr[0];
                                                accelerations[3*j+1] -= force*dr[1];
                                                accelerations[3*j+2] -= force*dr[2];
                                            }

                                        }
                                    } // if( i != j) {

                                    j = linked_list_all_atoms[j];
                                } // while (j != EMPTY) {
                                i = linked_list_all_atoms[i];

                            } // while (i != EMPTY) {


                            // cout << "Selected new i: " << i << endl;
                        } // for mc1[2]
                    } // for mc1[1]
                } // for mc1[0]
            } // for mc[2]
        } // for mc[1]
    } // for mc[0]
    // cout << "Atoms: " << potential_energy_count << endl;
    pressure_forces /= mass_inverse;
}
