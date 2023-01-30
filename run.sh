#!/bin/bash

make -j4
GRAPH="CollegeMsg"

# First preprocess input graphs from .el to .txt
# for GRAPH in "CollegeMsg" "email-Eu-core-temporal" "sx-mathoverflow" "sx-askubuntu" "sx-superuser" "wiki-talk-temporal" "sx-stackoverflow"
# do
#     python3.6 preprocess_input.py -i graphs/${GRAPH}.el
# done

# Mine triangles in GRAPH with benchmarking (-b)
# for GRAPH in "CollegeMsg" "email-Eu-core-temporal" "sx-mathoverflow" "sx-askubuntu" "sx-superuser" "wiki-talk-temporal" "sx-stackoverflow"
for GRAPH in "CollegeMsg" "email-Eu-core-temporal" "wiki-talk-temporal" "sx-stackoverflow"
do
    # echo "Graph="${GRAPH} "delta=3600"
    ./ettc graphs/${GRAPH}.txt 3600 3600 3600 ${GRAPH}_out.txt #-b | tail -1
done