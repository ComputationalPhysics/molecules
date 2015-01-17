# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import *
from sys import argv
from generators import *

fileName = argv[1]

lammps_header = zeros(1, dtype=lammps_header_dtype)

fcc_b = 1.562
nx,ny,nz = 14,5,5

Lx,Ly,Lz = nx*fcc_b,ny*fcc_b,nz*fcc_b

lammps_header["bounds"][0][0] = 0
lammps_header["bounds"][0][1] = Lx
lammps_header["bounds"][0][2] = 0
lammps_header["bounds"][0][3] = Ly
lammps_header["bounds"][0][4] = 0
lammps_header["bounds"][0][5] = Lz

atoms = create_fcc(nx, ny, nz, T=0.02)

index = 0
for atom in atoms:
    atom["id"] = index + 1
    index += 1

save_atoms(lammps_header, atoms, fileName)