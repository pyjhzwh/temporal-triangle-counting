#!/bin/bash

make -j4
GRAPH="CollegeMsg"
delta=3600 # 60 3600 86400

# First preprocess input graphs from .el to .txt
# for GRAPH in "CollegeMsg" "email-Eu-core-temporal" "sx-mathoverflow" "sx-askubuntu" "sx-superuser" "wiki-talk-temporal" "sx-stackoverflow"
# do
#     python3.6 preprocess_input.py -i graphs/${GRAPH}.el
# done

# Mine triangles in GRAPH with benchmarking (-b)
#for GRAPH in "CollegeMsg" "email-Eu-core-temporal" "sx-mathoverflow" "sx-askubuntu" "sx-superuser" "wiki-talk-temporal" "sx-stackoverflow"
#do
    #for delta in 60 3600 86400
    #do
        echo "Graph="${GRAPH} "delta="${delta}
        ./ettc graphs/${GRAPH}.txt ${delta} ${delta} ${delta} ${GRAPH}_out.txt -b #| tail -1
    #done
#done