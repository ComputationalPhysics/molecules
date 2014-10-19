# -*- coding: utf-8 -*-

from pylab import *
from lammpsreader import load_atoms, save_atoms
from sys import argv
fileName = argv[1]
lammpsHeader, atoms, system_lengths = load_atoms(fileName)

cylinderRadius = 3.0

nFixed = 0
for atom in atoms:
    diff = (atom["position"] - system_lengths / 2)
    if(sqrt(diff[0]**2 + diff[1]**2) < cylinderRadius):
        atom["type"] = 18
        atom["velocity"][0] = 0.0
        atom["velocity"][1] = 0.0
        atom["velocity"][2] = 1.0
    else:
        atom["type"] = 0
        atom["velocity"][0] = 0.0
        atom["velocity"][1] = 0.0
        atom["velocity"][2] = 0.0
        nFixed += 1
    
print "Fixed", nFixed, "of", len(atoms), "atoms"
save_atoms(lammpsHeader, atoms, argv[2])
