# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import load_atoms, save_atoms
from sys import argv
from generators import *

fileName = argv[1]
lammpsHeader, atoms, system_lengths = load_atoms(fileName)

cylinderRadius = system_lengths[1] * 0.4

nFixed = 0
free_count = 0
for atom in atoms:
    diff = (atom["position"] - system_lengths / 2)
    if(sqrt(diff[1]**2 + diff[2]**2) < cylinderRadius):
        if free_count > len(atoms)/15:
            atom["position"][0] = 1337.1
        atom["type"] = 18
        atom["velocity"][0] = 2.0
        atom["velocity"][1] = 0.0
        atom["velocity"][2] = 0.0
        free_count += 1
    else:
        atom["type"] = 1
        atom["velocity"][0] = 0.0
        atom["velocity"][1] = 0.0
        atom["velocity"][2] = 0.0
        nFixed += 1

print "Atoms before delete: ", len(atoms)
atoms = delete(atoms, where(atoms["position"][:,0] > 1337.0))
print "Atoms after delete: ", len(atoms)
    
print "Fixed", nFixed, "of", len(atoms), "atoms"
save_atoms(lammpsHeader, atoms, argv[2])
