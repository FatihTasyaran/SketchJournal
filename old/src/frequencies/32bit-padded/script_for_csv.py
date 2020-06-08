import pandas as pd
import sys
import numpy as np

def concat_me(nmreal,nm1,nm2,nm4,oname):
    filereal = open(nmreal,"r")
    file1 = open(nm1,"r")
    file2 = open(nm2,"r")
    file4 = open(nm4,"r")

    lisreal = []
    lis1 = []
    lis2 = []
    lis4 = []

    for row in filereal:
        lisreal.append(row[0:-1])
    
    testreal=pd.DataFrame(lisreal,columns=["Real"])
    testreal.reset_index(drop=True, inplace=True)
    for row in file1:
        lis1.append(row[0:-1])
    
    test1=pd.DataFrame(lis1,columns=["Single Thread"])
    test1.reset_index(drop=True, inplace=True)
    for row in file2:
        lis2.append(row[0:-1])
    
    test2=pd.DataFrame(lis2,columns=["2 Thread"])
    test2.reset_index(drop=True, inplace=True)
    for row in file4:
        lis4.append(row[0:-1])
    
    test4=pd.DataFrame(lis4,columns=["4 Thread"])
    test4.reset_index(drop=True, inplace=True)

    
    q=pd.concat([testreal,test1,test2,test4],axis=1)

    
    q.to_csv(oname)
    print("done")

def main(args):
    if len(args)==5:
        filereal=args[1]
        file1=args[2]
        file2=args[3]
        file4=args[4]
        oname = args[5]
        concat_me(filereal,file1,file2,file4,oname)
    else:
        prnp.int("Not enough arguments. Please provide 4 file names and a result file name.")
if __name__ ==  "__main__":
    main(sys.argv)
