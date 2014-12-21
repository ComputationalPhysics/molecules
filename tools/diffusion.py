# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import *
from sys import argv

fileName = argv[1]

lammps_header = zeros(1, dtype=lammps_header_dtype)

Lx,Ly,Lz = 20.,10.,10.

lammps_header["bounds"][0][0] = 0
lammps_header["bounds"][0][1] = Lx
lammps_header["bounds"][0][2] = 0
lammps_header["bounds"][0][3] = Ly
lammps_header["bounds"][0][4] = 0
lammps_header["bounds"][0][5] = Lz

nx,ny,nz = 9,7,7

atoms = zeros(0, dtype=atoms_dtype)

ratios = [0.35, 0.20, 0.35]
shifts = [0.00, 0.35, 0.55]

atoms_live = create_fcc(ratios[0]*nx, ny, nz, ratios[0]*Lx, Ly, Lz, atom_type=18, T=0.001)
atoms_live2 = create_fcc(ratios[1]*nx, ny, nz, ratios[1]*Lx, Ly, Lz, atom_type=181, T=0.001)
atoms_live3 = create_fcc(ratios[2]*nx, ny, nz, ratios[2]*Lx, Ly, Lz, atom_type=18, T=0.001)

# Move FCC out
for atom in atoms_live:
    atom["position"][0] += shifts[0] * Lx + 0.5
for atom in atoms_live2:
    atom["position"][0] += shifts[1] * Lx + 0.5
for atom in atoms_live3:
    atom["position"][0] += shifts[2] * Lx + 0.5

atoms = concatenate((atoms, atoms_live))
atoms = concatenate((atoms, atoms_live2))
atoms = concatenate((atoms, atoms_live3))

index = 0
for atom in atoms:
    atom["id"] = index + 1
    index += 1

save_atoms(lammps_header, atoms, fileName)
