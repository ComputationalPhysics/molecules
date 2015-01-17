from pylab import *
from sys import argv
from os.path import expanduser
from lammpsreader import save_atoms, load_atoms
from generators import *

class Ball:
    radius = 0
    center = array([0,0,0])

fileName = expanduser(argv[1])
lammpsHeader, atoms, system_lengths = load_atoms(fileName)

print "Loaded ", len(atoms), " atoms"
    
directionalVectors = []

for i in range(-1,2):
    for j in range(-1,2):
        for k in range(-1,2):
            directionalVector = system_lengths * array([i,j,k])
            directionalVectors.append(directionalVector)

balls = []
nPoresSet = [8, 16, 20, 24, 28, 32, 64]
for nPores in nPoresSet:
    while len(balls) <= nPores:
        ball = Ball()
        overlapping = (len(balls) > 0)
        ball.radius = random() * 1.0 + 2.0
        attempts = 0
    #    while overlapping:
        overlapping = False
        ball.center = random(3) * (system_lengths)
        for otherBall in balls:
            for directionalVector in directionalVectors:
                vector = (otherBall.center - ball.center + directionalVector)
                if norm(vector) < (ball.radius +  otherBall.radius):
                    overlapping = True
    
        if attempts > 1e3:
            print "Tried too many times, restarting pore list..."
            balls = []
            overlapping = False
        attempts += 1
        
        balls.append(ball)
     
    iBall = 0
    atoms["type"] = atoms["type"] * 0
    for ball in balls:
        iDirection = 0
        nFixedBall = 0
        for directionalVector in directionalVectors:
            vector = atoms["position"] - ball.center + directionalVector
            dotProducts = sum(vector * vector, axis=1)
            fixedPositions = (dotProducts < ball.radius**2)
            atoms["type"] = (atoms["type"] + fixedPositions)
        iBall+=1
    atoms["type"] = (atoms["type"] > 0) * 18
    for atom in atoms:
        if atom["type"] == 0:
            atom["velocity"][0] = 0
            atom["velocity"][1] = 0
            atom["velocity"][2] = 0
    nFixed = int(sum(atoms["type"] > 0))
    print "Fixed", nFixed, "of", len(atoms), "atoms"
    outFileName = argv[2]
    outFileName = outFileName.replace(".lmp", str(nPores) + ".lmp")
    save_atoms(lammpsHeader, atoms, outFileName)