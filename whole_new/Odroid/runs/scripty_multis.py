import math,sys
import csv

dist = sys.argv[1]
ver  = sys.argv[2]

repeat = 5
num_rounds = 4

header = ['TAKE', '1THR', '2THR', '4THR', '8THR']
col_sizes = ['0.01', '0.001', '0.0001', '0.00001']

##DECLARATIONS##
time_array_naive = []
through_array_naive = []
err_array_naive = []

time_array_merged = []
through_array_merged = []
err_array_merged = []

time_array_cont = []
through_array_cont = []
err_array_cont = []


naives = [time_array_naive, through_array_naive, err_array_naive]
mergeds = [time_array_merged, through_array_merged, err_array_merged]

def oneDance(op,array):
    ##NEBULA/GANDALF##
    time1beg = op.find("time:")
    time1beg += 5 ##omit "time"
    time1end = op.find("through", time1beg+1, len(op))
    time1end -= 2 ##omit "\t"
    time1 = op[time1beg:time1end]
    array[0].append(time1)

    
    through1beg = time1end + 10 ##omit "through"
    through1end = op.find("error", through1beg+1, len(op))
    through1end -= 2 ##omit "\t"
    through1 = op[through1beg:through1end]
    array[1].append(through1)

    
    error1beg = through1end + 8 ##omit "error"
    error1end = op.find("top_item", error1beg+1, len(op))
    error1end -= 2 ##omit "\t"
    error1 = op[error1beg:error1end]
    array[2].append(error1)

    return error1end

def appendName(name, filename):
    filer = open(filename, "a")
    filer.write(name)
    filer.write("\n")

def appendNames(name1, name2, filename):
    filer = open(filename, "a")
    filer.write(name1)
    filer.write(",")
    filer.write(name2)
    filer.write("\n")


##DECLARATIONS##

def headerW(writer, header):
    with open(writer, "w") as fp:
        csv_writer = csv.writer(fp, delimiter=',')
        csv_writer.writerow(header)
        header = [' ']
        csv_writer.writerow(header)
    

##CSV##
for col in col_sizes:
    files = dist + "_"+ col + "_" + ver
    reader = files + ".txt"
    writer = ver + ".csv"

    if(col == "0.01"):
        headerW(writer, header)
    
    appendNames('COLSIZE:', col, writer)
    
    op = open(reader, "r").read()

    for r in range (num_rounds):

        last = oneDance(op, naives)
        op = op[last:len(op)]

    print(naives)        
        
    for r in range (num_rounds):

        last = oneDance(op, mergeds)
        op = op[last:len(op)]

    print(mergeds)
