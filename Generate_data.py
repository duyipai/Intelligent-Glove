#! /usr/bin/env python3
import sys

# To use this script, specify one file, like
# 'python Generate_data.py sample_data.txt'
# The output file is saved as 'data.txt' in current directory 

if len(sys.argv) != 2:
    print('Please specify input file')
    exit()

in_file = open(sys.argv[1])
out_file = open('data.txt', 'w')

for i, line in enumerate(in_file):
    # print(line)
    words = line.split()
    words = words[-3:]
    out_file.write(' '.join(words))
    if i % 2 == 1:
        out_file.write('\n')
    else:
        out_file.write(' ')
in_file.close()
out_file.close()