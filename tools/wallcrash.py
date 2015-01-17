# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import *
from sys import argv
from generators import *

fileName = argv[1]

lammps_header = zeros(1, dtype=lammps_header_dtype)

Lx,Ly,Lz = 50.,18.,18.

lammps_header["bounds"][0][0] = 0
lammps_header["bounds"][0][1] = Lx
lammps_header["bounds"][0][2] = 0
lammps_header["bounds"][0][3] = Ly
lammps_header["bounds"][0][4] = 0
lammps_header["bounds"][0][5] = Lz

nx,ny,nz = 10,20,20

atoms = zeros(nx*ny*nz, dtype=atoms_dtype)

index = 0

# First wall
for i in range(1):
    for j in range(ny):
        for k in range(nz):
            x = Lx * 0.5
            y = float(j + 0.5) / ny * Ly
            z = float(k + 0.5) / nz * Lz
            atoms[index]["position"][0] = x
            atoms[index]["position"][1] = y
            atoms[index]["position"][2] = z
            atoms[index]["type"] = 0
            index += 1

atoms = resize(atoms, index)

atoms_live =  zeros(8, dtype=atoms_dtype)
#atoms_live = create_fcc(2, 2, 2, 3, 3, 3, atom_type=18, T=0.05)

index = 0
for i in range(2):
    for j in range(2):
        for k in range(2):
            atom = atoms_live[index]
            atom["position"][0] += Lx * 0.05 + i * 1
            atom["position"][1] += 0.5 * Ly - 0.5 + j * 1
            atom["position"][2] += 0.5 * Lz - 0.5 + k * 1
            atom["velocity"][0] = 5.0
            atom["type"] = 18
            index += 1

atoms = concatenate((atoms, atoms_live))

index = 0
for atom in atoms:
    atom["id"] = index + 1
    index += 1

save_atoms(lammps_header, atoms, fileName)
