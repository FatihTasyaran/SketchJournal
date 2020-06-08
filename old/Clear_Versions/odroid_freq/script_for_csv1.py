import pandas as pd
import sys
import numpy as np

nmreal=sys.argv[1]
nm4=sys.argv[2]
nm4bar=sys.argv[3]
nm8=sys.argv[4]
nm8bar=sys.argv[5]
oname = sys.argv[6]

filereal = open(nmreal,"r")
file4 = open(nm4,"r")
file4bar = open(nm4bar, "r")
file8 = open(nm8,"r")
file8bar = open(nm8bar, "r")
#file4 = open(nm4,"r")

lisreal = []
lis4 = []
lis4barrier = []
lis8 = []
lis8barrier = []

for row in filereal:
    lisreal.append(row[0:-1])

    
testreal=pd.DataFrame(lisreal,columns=["Real"])
testreal.reset_index(drop=True, inplace=True)


for row in file4:
    lis4.append(row[0:-1])

for row in file4bar:
    lis4barrier.append(row[0:-1])
    
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
