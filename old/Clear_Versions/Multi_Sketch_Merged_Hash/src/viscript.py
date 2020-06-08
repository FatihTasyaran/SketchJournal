import os, sys, subprocess, math
#import pandas as pd
import numpy as np
#import matplotlib
#import matplotlib.pyplot as plt
from functools import reduce
import csv

our_exec = sys.argv[1]
ssz = sys.argv[2]
sz = sys.argv[3]
col = sys.argv[4]
row = sys.argv[5]
maxt = sys.argv[6]
dist = sys.argv[7]
name_var = sys.argv[8]
repeat = sys.argv[9]
no_versions = sys.argv[10]

repeat = int(repeat)
no_threads = int(maxt)
no_results = math.log(no_threads, 2) + 1
no_results = int(no_results)
no_versions = int(no_versions)

thr1, thr2, thr4, thr8, thr16, err1, err2, err4, err8, err16 = [], [], [], [], [], [], [], [], [], []

names = []
times = [thr1, thr2, thr4, thr8, thr16]
errs = [err1, err2, err4, err8, err16]

version = [[names, times, errs] * int(no_versions)]

print('No Results: ', no_results, 'No versions: ', no_versions)

def oneIter(end, op):
    start = op.find(",", end, len(op))
    end = op.find(",", start + 1, len(op))
    for j in range(no_results):
        if(j < 4):
            threaddummy = op[start+1:end-1]
        else:
            threaddummy = op[start+1:end-2]
        print(threaddummy)
        if(threaddummy == "THR"):
            start = op.find(",", end, len(op))
            end = op.find(",", start + 1, len(op))
            timedummy = op[start + 1:end]
        if(timedummy == "TIME"):
            start = op.find(",", end, len(op))
            end = op.find(",", start+1, len(op))
            valtime = op[start+1:end]
            valtime = float(valtime)
            times[j].append(valtime)
            start = op.find(",", end, len(op))
            end = op.find(",", start+1, len(op))
            errdummy = op[start+1:end]
        else:
            print("Expected time identifier, found ", timedummy)
        if(errdummy == "ERR"):
            start = op.find(",", end, len(op))
            end = op.find("\n", start + 1, len(op))
            valerr = op[start+1:end]
            print(valerr, "SA")
            #print repr(valerr)
            valerr = float(valerr)
            errs[j].append(valerr)
        else:
            print("Expected error identifier, found ", errdummy)
        start = op.find(",", end, len(op))
        end = op.find(",", start + 1, len(op))
        #threaddummy = op[start+1:end]

affinity1 = "export OMP_PLACES_={7},{6},{5},{4},{3},{2},{1},{0}"
affinity2 = "export OMP_PROC_BIND=close"
for i in range(0, repeat):
    os.system(affinity1)
    os.system(affinity2)
    op = subprocess.check_output([our_exec, ssz, sz, col, row, maxt, dist])
    op = str(op)
    for j in range(0, no_versions):
        version_start = op.find("--VERSION")
        name_start = op.find(",",version_start+1,len(op))
        name_end = op.find(",",name_start+1,len(op))
        version_name = op[name_start+1:name_end]
        names.append(version_name)
        oneIter(name_end+1,op)
        op = op[version_start+1:len(op)]

#print(times)
#print(errs)
#print(version)

visarray_name = [[] for i in range(no_versions)]
visarray_thr1 = [[] for i in range(no_versions)]
visarray_thr2 = [[] for i in range(no_versions)]
visarray_thr4 = [[] for i in range(no_versions)]
visarray_thr8 = [[] for i in range(no_versions)]
visarray_thr16 = [[] for i in range(no_versions)]
visarray_err1 = [[] for i in range(no_versions)]
visarray_err2 = [[] for i in range(no_versions)]
visarray_err4 = [[] for i in range(no_versions)]
visarray_err8 = [[] for i in range(no_versions)]
visarray_err16 = [[] for i in range(no_versions)]

visarray = [visarray_name, visarray_thr1, visarray_thr2, visarray_thr4, visarray_thr8, visarray_thr16, visarray_err1, visarray_err2, visarray_err4, visarray_err8, visarray_err16]

for i in range(0, no_versions):
    visarray_name[i].append(names[i])

for i in range(0, repeat):
    for j in range(0, no_results):
        for k in range(0, no_versions):
            #print("Take",i,names[k],pow(2, j),"thread",times[j][k*i + (no_versions * k)])
            visarray[j+1][k].append(times[j][(no_versions * i) + k])
            visarray[j+6][k].append(errs[j][(no_versions * i) + k])

#print(len(visarray[4][1]), visarray[4][1])
#print(visarray[3][2][3])

dummy = 0.0
sum = 0.0

def Average(lst):
    dummy = 0
    for i in range(0, len(lst)):
        dummy += lst[i]
    return dummy / len(lst)


#for i in range(1, len(visarray)):
#    for j in range(0, no_versions):
#        dummy = Average(visarray[i][j])
#        visarray[i][j] = dummy

#print(visarray)

thread = np.array([1,2,4,8,16],dtype=str)

time = [np.array for i in range(repeat)]
vertime = [np.array for i in range(no_versions)]



threadlist = [1,2,4,8,16]

print(time)

for i in range(0, len(threadlist)):
    time[i] = np.array(visarray[int(math.log(threadlist[i], 2) + 1)])


dummylist = []
for i in range(0,no_versions):
    for j in range(0,repeat):
        val = time[j][i]
        dummylist.append(val)
    vertime[i] = np.array(dummylist)
    dummylist = []

print(type(thread), type(time))
print(thread)
print(time)

#ax = plt.subplots()
#plt.plot(thread, vertime[0])
#plt.plot(thread, vertime[1])
#plt.plot(thread, vertime[2])

#for i in range(0, no_versions):
#    plt.plot(thread, vertime[i])#, label=visarray[0][i])

#for i in range(0, no_versions):
#    plt.legend(visarray[0][i])

#plt.set_xlabel('Number of Threads')
#plt.set_ylabel('Time in seconds')


#plt.show()

filename = our_exec + name_var + ".csv"

with open(filename, 'w') as fp:
    a = csv.writer(fp)
    data = visarray_name + times
    a.writerows(data)


