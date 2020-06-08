from __future__ import print_function

import sys, subprocess, os

our_exec = sys.argv[8]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]

affinity1 = "export OMP_PLACES={7},{6},{5},{4},{3},{2},{1},{0}"
affinity2 = "export OMP_PROC_BIND=close"

affinity = affinity1 + affinity2

print("Start")
filename = ss + "_" + bs + "_" + str(col) + "_" + dist + "_" + name_var + ".txt"
file = open(filename, "a")

for i in range(10):
    os.system(affinity1)
    os.system(affinity2)
    op1 = subprocess.check_output([our_exec, ss, bs, col, row, maxt, dist])
    print("Done!")
    file.write(op1)

file.close()
