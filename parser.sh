#!/bin/bash

files=$(ls -1 ./outs/*.o)

for file in $files
do

	grep -E "DATA_FSP_V1" $file >> data.csv

done