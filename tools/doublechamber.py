# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import *
from sys import argv

fileName = argv[1]

lammps_header = zeros(1, dtype=lammps_header_dtype)

Lx,Ly,Lz = 16.,16.,16.

lammps_header["bounds"][0][0] = 0
lammps_header["bounds"][0][1] = Lx
lammps_header["bounds"][0][2] = 0
lammps_header["bounds"][0][3] = Ly
lammps_header["bounds"][0][4] = 0
lammps_header["bounds"][0][5] = Lz

nx,ny,nz = 16,16,16

atoms = zeros(nx*ny*nz, dtype=atoms_dtype)

index = 0

# First wall
for i in range(2):
    for j in range(ny):
        for k in range(nz):
            if i == 0:
                x = 0.5 * Lx / nx
            else:
                x = (nx - 0.5) * Lx / nx
                
            offset = (i % 2) * 0.5
            y = float(j + offset) / ny * Ly
            z = float(k + offset) / nz * Lz
            atoms[index]["position"][0] = x
            atoms[index]["position"][1] = y
            atoms[index]["position"][2] = z
            atoms[index]["type"] = 1
            index += 1
            
# Second wall (with hole)
for i in range(2):
    for j in range(ny):
        for k in range(nz):
            if i == 0:
                x = (nx / 2.0 - 0.5) * Lx / nx
            else:
                x = (nx / 2.0 + 0.5) * Lx / nx
                
            offset = (i % 2) * 0.5
            y = float(j + offset) / ny * Ly
            z = float(k + offset) / nz * Lz
            if (y / Ly < 0.4 or y / Ly >= 0.6) or (z / Lz < 0.4 or z / Lz >= 0.6):
                atoms[index]["position"][0] = x
                atoms[index]["position"][1] = y
                atoms[index]["position"][2] = z
                atoms[index]["type"] = 1
                index += 1

atoms = resize(atoms, index)

length_x = (nx / 2.0 - 1.0) * Lx / nx - 0.5 * Lx / nx
atoms_live = create_fcc(4, ny, nz, length_x, Ly, Lz, atom_type=18, T=0.05)

# Move FCC out
for atom in atoms_live:
    atom["position"][0] += Lx / nx * 0.75

atoms = concatenate((atoms, atoms_live))

index = 0
for atom in atoms:
    atom["id"] = index + 1
    index += 1

save_atoms(lammps_header, atoms, fileName)
