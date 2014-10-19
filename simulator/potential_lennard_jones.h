#include <system.h>
#include <settings.h>

/* Calculates the lennard jones potential between all free atoms within dr<=r_cut.
 * Skips all frozen atom pairs.
 * Calculates potential energy and virial term for pressure calculation.
 *
 */
void System::calculate_accelerations() {
    double rr_cut = r_cut*r_cut;

    for (c=0; c<num_cells_including_ghosts_xyz; c++) { head_all_atoms[c] = EMPTY; head_free_atoms[c] = EMPTY; }

    for (i=0; i<num_atoms+num_atoms_ghost; i++) {
        for (a=0; a<3; a++) mc[a] = (positions[3*i+a]+cell_length[a])/cell_length[a];
        cell_index_from_vector(mc,cell_index);

        // Set this atom at the head of the linked list, and update its next
        linked_list_all_atoms[i] = head_all_atoms[cell_index];
        head_all_atoms[cell_index] = i;
    }

    double mass_inverse_24 = mass_inverse*24;

    double r_cut_squared_inverse = 1.0/rr_cut;
    double r_cut_6_inverse = r_cut_squared_inverse*r_cut_squared_inverse*r_cut_squared_inverse;
    double potential_energy_correction = 4*r_cut_6_inverse*(r_cut_6_inverse - 1);

    // Loop through all local cells (not including ghosts)
    for (mc[0]=1; mc[0]<=num_cells_local[0]; mc[0]++) {
        for (mc[1]=1; mc[1]<=num_cells_local[1]; mc[1]++) {
            for (mc[2]=1; mc[2]<=num_cells_local[2]; mc[2]++) {
                cell_index = mc[0]*num_cells_including_ghosts_yz+mc[1]*num_cells_including_ghosts[2]+mc[2];
                if ( head_all_atoms[cell_index] == EMPTY ) continue;

                // Loop through all neighbors (including ghosts) of this cell.
                for (mc1[0]=mc[0]-1; mc1[0]<=mc[0]+1; mc1[0]++) {
                    for (mc1[1]=mc[1]-1; mc1[1]<=mc[1]+1; mc1[1]++) {
                        for (mc1[2]=mc[2]-1; mc1[2]<=mc[2]+1; mc1[2]++) {
                            cell_index_2 = mc1[0]*num_cells_including_ghosts_yz+mc1[1]*num_cells_including_ghosts[2]+mc1[2];

                            if(head_all_atoms[cell_index_2] == EMPTY) continue;
                            i = head_all_atoms[cell_index]; // Index of local atom i

                            while (i != EMPTY) {
                                j = head_all_atoms[cell_index_2]; // Index of atom j
                                while (j != EMPTY) {
                                    if(i < j) { // Newton's 3rd law
                                        dr[0] = positions[3*i+0]-positions[3*j+0];
                                        dr[1] = positions[3*i+1]-positions[3*j+1];
                                        dr[2] = positions[3*i+2]-positions[3*j+2];
                                        double dr2 = dr[0]*dr[0] + dr[1]*dr[1] + dr[2]*dr[2];

                                        if (dr2<rr_cut) {
                                            int precomputedTableIndex = dr2*one_over_r_cut_squared*numberOfPrecomputedTwoParticleForces;

                                            double force0 = precomputed_forces[precomputedTableIndex];
                                            double force1 = precomputed_forces[precomputedTableIndex+1];
                                            // Linearly interpolate between these values
                                            double force = force0 + (force1 - force0)*(dr2 - precomputedTableIndex*deltaR2)*oneOverDeltaR2;

                                            bool is_local_atom = j < num_atoms; // Ghost atoms contributes with 0.5 of pressure and potential energy statistics
//                                            double dr2_inverse = 1.0/dr2;
//                                            double dr6_inverse = dr2_inverse*dr2_inverse*dr2_inverse;

//                                            double force2 = (2*dr6_inverse-1)*dr6_inverse*dr2_inverse*mass_inverse_24;

                                            if(sample_statistics) {
                                                double energy0 = precomputed_potential[precomputedTableIndex];
                                                double energy1 = precomputed_potential[precomputedTableIndex+1];
                                                double potential_energy_tmp = energy0 + (energy1 - energy0)*(dr2 - precomputedTableIndex*deltaR2)*oneOverDeltaR2;
                                                //double potential_energy_tmp = 4*dr6_inverse*(dr6_inverse - 1) - potential_energy_correction;
                                                if(is_local_atom) {
                                                    potential_energy += potential_energy_tmp;
                                                    pressure_forces += force*dr2;
                                                } else {
                                                    pressure_forces += 0.5*force*dr2;
                                                    potential_energy += 0.5*potential_energy_tmp;
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
