#! /usr/bin/env python3
import sys

# To use this script, specify one file, like
# 'python Generate_data.py sample_data.txt'
# The output file is saved as 'data.txt' in current directory

if len(sys.argv) != 2:
    print('Please specify input file')
    exit()

in_file = open(sys.argv[1])
file_name = (((sys.argv[1]).replace('\\', '.').replace('/',
                                                       '.').split('.')))[-2]
out_file = open(file_name + '_out.txt', 'w')

for i, line in enumerate(in_file):
    # print(line)
    words = line.split()
    words = words[-9:]
    out_file.write(' '.join(words))
    # if i % 2 == 1:
    #     out_file.write('\n')
    # else:
    #     out_file.write(' ')
    out_file.write('\n')
in_file.close()
out_file.close()