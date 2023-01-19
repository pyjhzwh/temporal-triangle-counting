# this script is to renumber the vertices in *.el file

import argparse
import re

parser = argparse.ArgumentParser()
parser.add_argument('-i', required=True, type=str, help="input graph path to be processed")
args = parser.parse_args()

input_file = args.i
input_name = (input_file).split(".")[0]
output_file = input_name + ".txt"

output_fp = open(output_file, "w")

time_stamp_list = []
old_nodes = set()
num_edges = 0

with open(input_file) as fp:
    print("Reading from input graph", input_file)
    for ln in fp:
        num_edges += 1
        tmp_list = []
        tmp_list.append(re.findall(r'\d+', ln))
        if len(tmp_list[0]) >= 3:       
            # format: src dst timestamp
            old_nodes.add(int(tmp_list[0][0]))
            old_nodes.add(int(tmp_list[0][1]))
        else:
            # unknown format
            sys.exit("[ERROR] Unknown number of columns in the data set!!")

sorted_old_nodes = sorted(old_nodes)
num_nodes = len(sorted_old_nodes)

old2new_node_mapping = {sorted_old_nodes[i]: i for i  in range(num_nodes)}
output_fp.write(f"{num_nodes} {num_edges}\n")

with open(input_file) as fp:
    print("Writing to file", output_file)
    for ln in fp:
        tmp_list = []
        tmp_list.append(re.findall(r'\d+', ln))
        if len(tmp_list[0]) == 3:
            # format: src dst timestamp
            new_src = old2new_node_mapping[int(tmp_list[0][0])]
            new_dst = old2new_node_mapping[int(tmp_list[0][1])]
            timestamp = tmp_list[0][2]
        elif len(tmp_list[0]) == 4:
            # format: src dst smt timestamp (smt = something I don't care about)
            new_src = old2new_node_mapping[int(tmp_list[0][0])]
            new_dst = old2new_node_mapping[int(tmp_list[0][1])]
            timestamp = tmp_list[0][3]
        else:
            # unknown format
            sys.exit("[ERROR] Unknown number of columns in the data set!!")
        output_fp.write((f"{new_src} {new_dst} {timestamp}\n"))
output_fp.close()