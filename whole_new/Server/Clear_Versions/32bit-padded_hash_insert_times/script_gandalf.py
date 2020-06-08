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


filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "0batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01" 


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "0batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename
print"One Down!"

our_exec = sys.argv[10]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]
distvar1 = sys.argv[8]


for i in range(5):
    os.system(arg)
    print"Done!"

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "1batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01"


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "1batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print"Done!"
print"One Down!"

our_exec = sys.argv[11]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]
distvar1 = sys.argv[8]


filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "2batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01"


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "2batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print"Done!"
print"One Down!"

our_exec = sys.argv[12]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]
distvar1 = sys.argv[8]


filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "3batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01"


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "3batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print"Done!"
print"One Down!"

our_exec = sys.argv[13]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]
distvar1 = sys.argv[8]


filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "4batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01"


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "4batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print"Done!"
print"One Down!"

our_exec = sys.argv[14]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]
distvar1 = sys.argv[8]


filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "8batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01"


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "8batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print"Done!"
print"One Down!"

our_exec = sys.argv[15]
ss = sys.argv[1]
bs = sys.argv[2]
col = sys.argv[3]
row = sys.argv[4]
maxt = sys.argv[5]
dist = sys.argv[6]
name_var = sys.argv[7]
distvar1 = sys.argv[8]


filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "16batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print "Done!"


if col == "0.01":
   col = "0.001"
else:
   col = "0.01"


print "column size changed to: ", col

filename = ss + "_" + bs + "_" + col + "_" + distvar1 + "_" + "16batch" + ".txt"

arg = our_exec + " " + ss + " " + bs + " " + col + " " + row + " " + maxt + " " + dist + " &>> " + filename

for i in range(5):
    os.system(arg)
    print"Done!"
print"One Down!"

print"All done!"



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
print "Completed!"
