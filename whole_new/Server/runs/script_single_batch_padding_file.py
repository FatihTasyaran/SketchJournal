import math, sys
import csv

dist = "1.5"
ver  = sys.argv[1]

repeat = 5
num_rounds = 4

header = ['TAKE', '1THR', '2THR', '4THR', '8THR']
col_sizes = ['0.00001', '0.000001']

##DECLARATIONS##
time_array_naive = []
hash_array_naive = []
write_array_naive = []
through_array_naive = []
err_array_naive = []

time_array_merged = []
hash_array_merged = []
write_array_merged = []
through_array_merged = []
err_array_merged = []

naives = [time_array_naive, hash_array_naive, write_array_naive, through_array_naive, err_array_naive]
mergeds = [time_array_merged, hash_array_merged, write_array_merged, through_array_merged, err_array_merged]
##DECLARATIONS##

def oneDance(op, array):
    time1beg = op.find("time:")
    time1beg += 5 ##omit time
    time1end = op.find("htime", time1beg+1, len(op))
    time1end -= 1 ##omit "\t"
    time1 = op[time1beg:time1end]
    array[0].append(time1)

    htime1beg = time1end + 7 ##omit "htime"
    htime1end = op.find("wtime", htime1beg+1, len(op))
    htime1end -= 2 ##omit "\t"
    htime1 = op[htime1beg:htime1end]
    array[1].append(htime1)

    wtime1beg = htime1end + 8 ##"omit wtime"
    wtime1end = op.find("through", wtime1beg + 1, len(op))
    wtime1end -= 2 ##omit "\t"
    wtime1 = op[wtime1beg:wtime1end]
    array[2].append(wtime1)
    
    through1beg = wtime1end + 11 ##omit "through"
    through1end = op.find("error", through1beg+1, len(op))
    through1end -= 1 ##omit "\t"
    through1 = op[through1beg:through1end]
    array[3].append(through1)

    error1beg = through1end + 8 ##omit "error"
    error1end = op.find("top_item", error1beg+1, len(op))
    error1end -= 2 ##omit "\t"
    error1 = op[error1beg:error1end]
    array[4].append(error1)

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
    files = ver + "_" +col
    reader = files + ".txt"
    writer = dist + ver + ".csv"

    if(col == "0.00001"):
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

    print("Naives:", naives[2])
    print("Mergeds:", mergeds[2])

    for i in range(5):

        appendName("BATCH", writer)
        
        if(i == 0):
            appendName("TIME:", writer)
        if(i == 1):
            appendName("HASH TIME:", writer)
        if(i == 2):
            appendName("WRITE TIME:", writer)
        if(i == 3):
            appendName("THROUGHPUT:", writer)
        if(i == 4):
            appendName("ERROR:", writer)
            
        appendOneColVERSION(writer, naives[i])

    for i in range(5):

        appendName("BATCH AND PADDING", writer)

        if(i == 0):
            appendName("TIME:", writer)
        if(i == 1):
            appendName("HASH TIME:", writer)
        if(i == 2):
            appendName("WRITE TIME:", writer)
        if(i == 3):
            appendName("THROUGHPUT:", writer)
        if(i == 4):
            appendName("ERROR:", writer)
        
        
        appendOneColVERSION(writer, mergeds[i])

    time_array_naive = []
    hash_array_naive = []
    write_array_naive = []
    through_array_naive = []
    err_array_naive = []

    time_array_merged = []
    hash_array_merged = []
    write_array_merged = []
    through_array_merged = []
    err_array_merged = []

    naives = [time_array_naive, hash_array_naive, write_array_naive, through_array_naive, err_array_naive]
    mergeds = [time_array_merged, hash_array_merged, write_array_merged, through_array_merged, err_array_merged]

    

