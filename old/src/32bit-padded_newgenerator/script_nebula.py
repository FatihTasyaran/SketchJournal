from __future__ import print_function

import sys, subprocess, os

our_exec = "./pis"
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name =sys.argv[7]



filename =str(name) + ss + "_" + bs + "_" + str(col) + "_" + dist + ".txt"
file = open(filename, "a")

for i in range(10):
    op1 = subprocess.check_output([our_exec, ss, bs, col, row, maxt, dist])
    print("Done!")
    file.write(op1)

file.close()


print("Experiment completed smoothly!")
