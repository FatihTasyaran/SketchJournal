import os, sys

our_exec = "./pis"
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]

filename = ss + "_" + bs + "_" + col + "_" + dist + "_" + name_var + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(10):
    os.system(arg)
    print "Done!"

print "Experiment Completed!"
