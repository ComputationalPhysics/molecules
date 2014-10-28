# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import *
from sys import argv

fileName = argv[1]

lammps_header = zeros(1, dtype=lammps_header_dtype)

mismatch = 1.2

x_scale = 3.0
y_scale = 1.0
z_scale = 2.0

Lx,Ly,Lz = x_scale*15.26, y_scale*10, mismatch*z_scale*15.26
#Lx,Ly,Lz = 1.3*15.26, 10.0, 1.3*15.26
fcc_Lx,fcc_Ly,fcc_Lz = Lx, 1.526, Lz

lammps_header["bounds"][0][0] = 0
lammps_header["bounds"][0][1] = Lx
lammps_header["bounds"][0][2] = 0
lammps_header["bounds"][0][3] = Ly
lammps_header["bounds"][0][4] = 0
lammps_header["bounds"][0][5] = Lz

nx,ny,nz = int(x_scale*10), int(y_scale*1), int(z_scale*10)

index = 0
atoms = create_fcc(nx, ny, nz, fcc_Lx, fcc_Ly, fcc_Lz, atom_type=18, T=0.0001)
x_pos = atoms["position"][:,0]
y_pos = atoms["position"][:,1]
z_pos = atoms["position"][:,2]

y_pos += Ly / 2.0 - fcc_Ly / 2.0

x_remove = 2.0
z_remove = 2.0

center_indexes = where((x_pos > 0.5 * Lx - x_remove / 2.0) & (x_pos < 0.5 * Lx + x_remove / 2.0) & \
                       (z_pos > 0.5 * Lz - z_remove / 2.0) & (z_pos < 0.5 * Lz + z_remove / 2.0))
atoms = delete(atoms, center_indexes)

index = 0
for atom in atoms:
    atom["id"] = index + 1
    index += 1

save_atoms(lammps_header, atoms, fileName)
