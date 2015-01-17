# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import *
from sys import argv
from generators import *

fileName = argv[1]

lammps_header = zeros(1, dtype=lammps_header_dtype)

Lx,Ly,Lz = 160.,18.,18.

lammps_header["bounds"][0][0] = 0
lammps_header["bounds"][0][1] = Lx
lammps_header["bounds"][0][2] = 0
lammps_header["bounds"][0][3] = Ly
lammps_header["bounds"][0][4] = 0
lammps_header["bounds"][0][5] = Lz

nx,ny,nz = 10, int(Ly/1.56), int(Lz/1.56)

index = 0

# First wall
atoms = create_fcc(2, ny, nz, T=0.0)
for atom in atoms:
    atom["position"][0] += Lx / 2.0
    
atoms_live = create_atoms(8)
#atoms_live = create_fcc(1, 1, 1, T=0.0)
#atoms_live = zeros(1, dtype=atoms_dtype)

counter = 0
for atom in atoms_live:
    atom["position"][0] += Lx / 4.0 + counter
    atom["position"][1] += Ly / 2.0
    atom["position"][2] += Lz / 2.0
    atom["velocity"][0] = 5.0
    atom["type"] = 18
    counter += 1

atoms = concatenate((atoms, atoms_live))

index = 0
for atom in atoms:
    atom["id"] = index + 1
    index += 1

save_atoms(lammps_header, atoms, fileName)
