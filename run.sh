#!/bin/bash
#
# Use this shell script to compile (if necessary) your code and then execute it. Below is an example of what might be found in this file if your program was written in Python
#
#python ./src/sessionization.py ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt
g++ -std=c++11 -g -o ./src/sessionization ./src/sessionization.cpp && \
./src/sessionization ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt

