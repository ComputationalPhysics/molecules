# -*- coding: utf-8 -*-
from pylab import *
from os.path import expanduser
from lammpsreader import *
from random import gauss, random
    
def create_atoms(atom_count):
    atoms = zeros(atom_count, dtype=atoms_dtype)
    return atoms

def create_fcc(nx, ny, nz, Lx=0, Ly=0, Lz=0, atom_type=18, T=0.0):
    b = 1.562
    nx = int(nx)
    ny = int(ny)
    nz = int(nz)
    if Lx == 0:
        Lx = nx*b
    if Ly == 0:
        Ly = ny*b
    if Lz == 0:
        Lz = nz*b
    x_vector = [0.0, 0.5, 0.5, 0.0]
    y_vector = [0.0, 0.5, 0.0, 0.5]
    z_vector = [0.0, 0.0, 0.5, 0.5]
    atoms = create_atoms(4*nx*ny*nz)
    for i in range(nx):
        for j in range(ny):
            for k in range(nz):
                for l in range(4):
                    index = i*ny*nz*4 + j*nz*4 + k*4 + l
                    x = float(i) / nx * Lx + x_vector[l] / nx * Lx
                    y = float(j) / ny * Ly + y_vector[l] / ny * Ly
                    z = float(k) / nz * Lz + z_vector[l] / nz * Lz
                    atoms[index]["position"][0] = x + Lx / nx / 4
                    atoms[index]["position"][1] = y + Ly / ny / 4
                    atoms[index]["position"][2] = z + Lz / nz / 4
                    
                    atoms[index]["velocity"][0] = gauss(0, 1) * sqrt(T)
                    atoms[index]["velocity"][1] = gauss(0, 1) * sqrt(T)
                    atoms[index]["velocity"][2] = gauss(0, 1) * sqrt(T)
                
                    atoms[index]["type"] = atom_type
    return atoms