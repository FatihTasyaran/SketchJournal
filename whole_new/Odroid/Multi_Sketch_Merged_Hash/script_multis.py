import os, sys ,subprocess, math
import csv

our_exec = sys.argv[1] ## Executable name 
ssz      = sys.argv[2] ## Super set size
sz       = sys.argv[3] ## Set size
col      = sys.argv[4] ## Column size
row      = sys.argv[5] ## Row size
maxt     = sys.argv[6] ## Max threads
dist     = sys.argv[7] ## Distribution
zipf     = sys.argv[8]
repeat   = sys.argv[9]
name_var = sys.argv[10]

##CASTINGS##
repeat = int(repeat)
maxt_int = int(maxt)
##CASTINGS##

##CALCULATIONS##
num_rounds = math.log(maxt_int, 2) + 1
num_rounds = int(num_rounds)
##CALCULATIONS##

##CSV##
if(dist == "2"):
    filename = name_var + "_uniform.csv"
else:
    filename = name_var + "_zipf_" + zipf + ".csv"
header = ['TAKE', '1THR', '2THR', '4THR', '8THR']
print(filename)
with open(filename, 'w') as fp:
    csv_writer = csv.writer(fp,delimiter=',')
    csv_writer.writerow(header)
    header = [' ']
    csv_writer.writerow(header)
    ##CSV##
    
##DECLARATIONS##
col_sizes = ['0.01', '0.001', '0.0001', '0.00001']

time_array_naive = []
through_array_naive = []
err_array_naive = []

time_array_merged = []
through_array_merged = []
err_array_merged = []

time_array_cont = []
through_array_cont = []
err_array_cont = []
##DECLARATIONS##



def appendName(name):
    filer = open(filename, "a")
    filer.write(name)
    filer.write("\n")

def appendNames(name1, name2):
    filer = open(filename, "a")
    filer.write(name1)
    filer.write(",")
    filer.write(name2)
    filer.write("\n")

def appendListofOneCOLSIZE(list):
    filer = open(filename, "a")


    for i in range(repeat):
        filer.write(str(i+1))
        filer.write(",")
        for item in list[i]:
            filer.write(item)
            filer.write(",")
        filer.write("\n")

def oneDance(op):
    ##NEBULA/GANDALF##
    time1beg = op.find("time:")
    time1beg += 5 ##omit "time"
    time1end = op.find("through", time1beg+1, len(op))
    time1end -= 2 ##omit "\t"
    time1 = op[time1beg:time1end]
    time_array_cont.append(time1)

    
    through1beg = time1end + 10 ##omit "through"
    through1end = op.find("error", through1beg+1, len(op))
    through1end -= 2 ##omit "\t"
    through1 = op[through1beg:through1end]
    through_array_cont.append(through1)

    
    error1beg = through1end + 8 ##omit "error"
    error1end = op.find("top_item", error1beg+1, len(op))
    error1end -= 2 ##omit "\t"
    error1 = op[error1beg:error1end]
    err_array_cont.append(error1)

    
    return error1end

###REAL LOOP###
for c in range(0, len(col_sizes)):
    
    appendNames('COLSIZE:', col_sizes[c])
    
    for re in range(repeat): ## FROM NOW ON, REPEAT TIME

        affinity1 = "export OMP_PLACES={7},{6},{5},{4},{3},{2},{1},{0}"
        affinity2 = "export OMP_PROC_BIND=close"
        os.system(affinity1)
        os.system(affinity2)
        op = subprocess.check_output([our_exec, ssz, sz, col_sizes[c], row, maxt, dist, zipf])
        op = str(op)
        
        for i in range(0, num_rounds): ## For each thread, for first version
            last = oneDance(op)
            op = op[last:len(op)]


        time_array_naive.append(time_array_cont)
        through_array_naive.append(through_array_cont)
        err_array_naive.append(err_array_cont)
            
        time_array_cont = []
        through_array_cont = []
        err_array_cont = []
            
        for i in range(0, num_rounds): ## For each thread, for second version
            last = oneDance(op)
            op = op[last:len(op)]
            ##print(op)
            
        time_array_merged.append(time_array_cont)
        through_array_merged.append(through_array_cont)
        err_array_merged.append(err_array_cont)
            
        time_array_cont = []
        through_array_cont = []
        err_array_cont = []

        ##WRITE ALL RESULTS OF ONE COLUMN SIZE TO CSV

    appendName('NAIVE')
    appendName('TIME')
    appendListofOneCOLSIZE(time_array_naive)
    appendName('ERROR')
    appendListofOneCOLSIZE(err_array_naive)
    appendName('THROUGHPUT')
    appendListofOneCOLSIZE(through_array_naive)

    appendName('MERGED')
    appendName('TIME')
    appendListofOneCOLSIZE(time_array_merged)
    appendName('ERROR')
    appendListofOneCOLSIZE(err_array_merged)
    appendName('THROUGHPUT')
    appendListofOneCOLSIZE(through_array_merged)

    time_array_naive = []
    err_array_naive = []
    through_array_naive = []

    time_array_merged = []
    err_array_merged = []
    through_array_merged = []
