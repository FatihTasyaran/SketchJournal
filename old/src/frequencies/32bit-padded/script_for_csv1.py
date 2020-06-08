import pandas as pd
import sys
import numpy as np

nmreal=sys.argv[1]
nm1 = sys.argv[2]
nm1bar = sys.argv[3]
nm2 = sys.argv[4]
nm2bar = sys.argv[5]
nm4=sys.argv[6]
nm4bar=sys.argv[7]
nm8=sys.argv[8]
nm8bar=sys.argv[9]
nm16 = sys.argv[10]
nm16bar = sys.argv[11]

oname = sys.argv

filereal = open(nmreal,"r")
file1 = open(nm1, "r")
file1bar = open(nm1bar, "r")
file2 = open(nm2, "r")
file2bar = open(nm2bar, "r")
file4 = open(nm4,"r")
file4bar = open(nm4bar, "r")
file8 = open(nm8,"r")
file8bar = open(nm8bar, "r")
file16 = open(nm16, "r")
file16bar = open(nm16bar, "r")
#file4 = open(nm4,"r")

lisreal = []
lis1 = []
lis1barrier = []
lis2 = []
lis2barrier = []
lis4 = []
lis4barrier = []
lis8 = []
lis8barrier = []
lis16 = []
lis16barrier = []

for row in filereal:
    lisreal.append(row[0:-1])

testreal=pd.DataFrame(lisreal,columns=["Real"])
testreal.reset_index(drop=True, inplace=True)

for row in file1:
    lis1.append(row[0:-1])

for row in file1bar:
    lis1barrier.append(row[0:-1])
    
test1=pd.DataFrame(lis4,columns=["1 Thread"])
test1.reset_index(drop=True, inplace=True)

test1barrier = pd.DataFrame(lis1barrier, columns=["1 Thread Barrier"])
test1barrier.reset_index(drop=True, inplace=True)

for row in file2:
    lis2.append(row[0:-1])

for row in file2bar:
    lis2barrier.append(row[0:-1])
    
test2=pd.DataFrame(lis2,columns=["2 Thread"])
test2.reset_index(drop=True, inplace=True)

test2barrier = pd.DataFrame(lis2barrier, columns=["2 Thread Barrier"])
test2barrier.reset_index(drop=True, inplace=True)

for row in file2:
    lis2.append(row[0:-1])

for row in file4bar:
    lis2barrier.append(row[0:-1])
    
test4=pd.DataFrame(lis4,columns=["4 Thread"])
test4.reset_index(drop=True, inplace=True)

test4barrier = pd.DataFrame(lis4barrier, columns=["4 Thread Barrier"])
test4barrier.reset_index(drop=True, inplace=True)

for row in file8:
    lis8.append(row[0:-1])

for row in file8bar:
    lis8barrier.append(row[0:-1])
    
test8=pd.DataFrame(lis8,columns=["8 Thread"])
test8.reset_index(drop=True, inplace=True)

test8barrier=pd.DataFrame(lis8barrier,columns=["8 Thread Barrier"])
test8barrier.reset_index(drop=True, inplace=True)

    
q=pd.concat([testreal,test4,test4barrier,test8,test8barrier],axis=1)

    
q.to_csv(oname)

print("done")
