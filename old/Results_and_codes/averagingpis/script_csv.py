import csv
import itertools
import sys

name = sys.argv[1]

with open(name + ".txt", 'r') as in_file:
    lines = in_file.read().splitlines()
    stripped = [line.replace(","," ").split() for line in lines]
    grouped = itertools.izip(*[stripped]*1)
    with open(name + ".csv", 'w') as out_file:
        writer = csv.writer(out_file)
        writer.writerow(('title', 'intro', 'tagline'))
        for group in grouped:
            writer.writerows(group)