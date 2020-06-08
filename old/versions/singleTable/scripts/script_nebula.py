from __future__ import print_function

import sys, subprocess, os

our_exec = "./pis"
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]


print("Start")
filename = ss + "_" + bs + "_" + str(col) + "_" + dist + "_" + name_var + ".txt"
file = open(filename, "a")

for i in range(10):
    op1 = subprocess.check_output([our_exec, ss, bs, col, row, maxt, dist])
    print("Done!")
    file.write(op1)

file.close()


#Change col size than repeat
if float(col) == 0.01:
    col = 0.001
else:
    col = 0.01
                        

filename = ss + "_" + bs + "_" + str(col) + "_" + dist + "_" + name_var + ".txt"
file = open(filename, "a")
print(col)

for i in range(10):
            op2 = subprocess.check_output([our_exec, ss, bs ,str(col), row, maxt, dist])
            print("Done!")
            file.write(op2)

file.close()

#Change distribution than repeat
if int(dist) == 1:
    dist = 2
else:
    dist = 1

print(dist)


filename = ss + "_" + bs + "_" + str(col) + "_" + str(dist) + "_" + name_var + ".txt"
file = open(filename, "a")

for i in range(10):
            op3 = subprocess.check_output([our_exec, ss, bs, str(col), row, maxt, str(dist)])
            print("Done!")
            file.write(op3)

file.close()


#Change col size again, total 4 files-> 0.01 normal...0.001 uniform
if(col == 0.001):
    col = 0.01
else:
    col = 0.001

print(col)


filename = ss + "_" + bs + "_" + str(col) + "_" + str(dist) + "_" + name_var + ".txt"
file = open(filename, "a")


for i in range(10):
            op4 = subprocess.check_output([our_exec, ss, bs, str(col), row, maxt,str(dist)])
            print("Done!")
            file.write(op4)
file.close()

print("Experiment Completed!")
