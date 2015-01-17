from pylab import *
from os.path import expanduser
from random import gauss, random

lammps_header_dtype = dtype([("step", 'int32'),
                          ("nAtoms", 'int32'),
                          ("bounds", float, (6,)),
                          ("shear", float, (3,)),
                          ("nColumns", 'int32'),
                          ("nChunks", 'int32'),
                          ("chunkLength", 'int32')])
atoms_dtype = dtype([("id", float),
                  ("type", float),
                  ("position", float, (3,)),
                  ("velocity", float, (3,))])

def load_atoms(fileName):
    fileName = expanduser(fileName)
    lammpsFile = open(fileName, "rb")
    lammpsHeader = fromfile(lammpsFile, dtype=lammps_header_dtype, count=1)
    atoms = fromfile(lammpsFile, dtype=atoms_dtype)
    lammpsFile.close()
    
#    atoms["position"] *= 1e-10

    if len(atoms) != lammpsHeader["nAtoms"]:
        raise Exception("Mismatching number of atoms in file. Header shows " + str(lammpsHeader["nAtoms"][0]) + " atoms, while we found " + str(len(atoms)) + " atoms in the file")
        
    system_lengths = array([
                        lammpsHeader["bounds"][0,1] - lammpsHeader["bounds"][0,0],
                        lammpsHeader["bounds"][0,3] - lammpsHeader["bounds"][0,2],
                        lammpsHeader["bounds"][0,5] - lammpsHeader["bounds"][0,4]
                       ])    
    
    return lammpsHeader, atoms, system_lengths
    
def save_atoms(lammpsHeader, atoms, fileName):
    fileName = expanduser(fileName)
    lammpsHeader["nColumns"] = atoms_dtype.itemsize / dtype('float64').itemsize
    print lammpsHeader["nColumns"], " columns"
    lammpsHeader["nAtoms"] = len(atoms)
    print lammpsHeader["nAtoms"], " atoms"
    lammpsHeader["nChunks"] = 1
    lammpsHeader["chunkLength"] = len(atoms) * lammpsHeader["nColumns"]
#    atoms["position"] *= 1e10 # Convert to LAMMPS units
    
    lammpsFile = open(fileName, "wb")
    lammpsHeader.tofile(lammpsFile)
    atoms.tofile(lammpsFile)
    lammpsFile.close()
    