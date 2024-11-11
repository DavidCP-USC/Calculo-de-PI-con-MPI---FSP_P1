#!/bin/bash

files=$(ls -1 ./P1.1.data/*.o)

for file in $files
do

	grep -E "DATA_FSP_V1" $file >> data.1.csv

done