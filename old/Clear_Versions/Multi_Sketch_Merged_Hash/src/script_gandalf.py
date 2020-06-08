import os, sys

our_exec = sys.argv[9]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]

distvar1 = sys.argv[8]
distvar2 = "uniform"

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + name_var + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01" 


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + name_var + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"

if dist == "1":
   dist = "2"
else:
   dist = "1"

#print "distiribution changed to: ", dist
#
#filename = ss + "_" + bs + "_" + col + "_" + distvar2 + "_" + name_var + ".txt"
#
#arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename
#
#for i in range(5):
#    os.system(arg)
#    print "Done!"
#
#if col == "0.001":
#   col = "0.01"
#else:
#   col = "0.001"
#
#print "column size changed to: ", col
#
#filename = ss + "_" + bs + "_" + col + "_" + distvar2 + "_" + name_var + ".txt"
#
#arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + "&>> " + filename
#
#for i in range(5):
#    os.system(arg)
#    print "Done!"
#
print "Experiment Completed!"
