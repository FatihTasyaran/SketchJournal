import csv
import itertools

with open('30_25_0.001_1_hwtimes.txt', 'r') as in_file:
    lines = in_file.read().splitlines()
    stripped = [line.replace(","," ").split() for line in lines]
    grouped = itertools.izip(*[stripped]*1)
    with open('30_25_0.001_1_hwtimes.csv', 'w') as out_file:
        writer = csv.writer(out_file)
        writer.writerow(('title', 'intro', 'tagline'))
        for group in grouped:
            writer.writerows(group)
