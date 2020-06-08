import math, sys
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

time_array_batched = []
through_array_batched = []
err_array_batched = []

naives = [time_array_naive, through_array_naive, err_array_naive]
mergeds = [time_array_merged, through_array_merged, err_array_merged]
batcheds = [time_array_batched, through_array_batched, err_array_batched]
##DECLARATIONS##

def oneDance(op, array):
    time1beg = op.find("time:")
    time1beg += 5 ##omit time
    time1end = op.find("through", time1beg+1, len(op))
    time1end -= 1 ##omit "\t"
    time1 = op[time1beg:time1end]
    array[0].append(time1)

    through1beg = time1end + 9 ##omit "through"
    through1end = op.find("error", through1beg+1, len(op))
    through1end -= 1 ##omit "\t"
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

def headerW(writer, header):
    with open(writer, "w") as fp:
        csv_writer = csv.writer(fp, delimiter = ",")
        csv_writer.writerow(header)
        header = [' ']
        csv_writer.writerow(header)


def appendOneColVERSION(filename, array):
    filer = open(filename, "a")
    
    for i in range(repeat * num_rounds):
        if(i%4 == 0):
            filer.write(str((i/4)+1))
            filer.write(",")
        filer.write(array[i])
        filer.write(",")
        if((i+1)%4 == 0):
            filer.write("\n")

        
for col in col_sizes:
    files = dist + "_" + col + "_" + ver
    reader = files + ".txt"
    writer = dist + ver + ".csv"

    if(col == "0.01"):
        headerW(writer, header)

    appendNames('COLSIZE:', col, writer)
    op = open(reader, 'r').read()

    for re in range(repeat): 
    
        for r in range(num_rounds):
            last = oneDance(op, naives)
            op = op[last:len(op)]

        for r in range(num_rounds):
            last = oneDance(op, mergeds)
            op = op[last:len(op)]

        for r in range(num_rounds):
            last = oneDance(op, batcheds)
            op = op[last:len(op)]

    print("Naives:", naives[0])
    print("Mergeds:", mergeds[0])

    for i in range(3):

        appendName("NAIVE", writer)
        
        if(i == 0):
            appendName("TIME:", writer)
        if(i == 1):
            appendName("THROUGHPUT:", writer)
        if(i == 2):
            appendName("ERROR:", writer)
            
        appendOneColVERSION(writer, naives[i])

    for i in range(3):

        appendName("MERGED", writer)
        
        if(i == 0):
            appendName("TIME:", writer)
        if(i == 1):
            appendName("THROUGHPUT:", writer)
        if(i == 2):
            appendName("ERROR:", writer)
        
        appendOneColVERSION(writer, mergeds[i])

    for i in range(3):

        appendName("BATCHED", writer)
        
        if(i == 0):
            appendName("TIME:", writer)
        if(i == 1):
            appendName("THROUGHPUT:", writer)
        if(i == 2):
            appendName("ERROR:", writer)
        
        appendOneColVERSION(writer, batcheds[i])

    time_array_naive = []
    through_array_naive = []
    err_array_naive = []

    time_array_merged = []
    through_array_merged = []
    err_array_merged = []

    time_array_batched = []
    through_array_batched = []
    err_array_batched = []

    naives = [time_array_naive, through_array_naive, err_array_naive]
    mergeds = [time_array_merged, through_array_merged, err_array_merged]
    batcheds = [time_array_batched, through_array_batched, err_array_batched]

    

