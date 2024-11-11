#!/bin/bash

rm -r ./outs
mkdir ./outs

module load gcc openmpi/4.1.4_ft3

# mpicc main.1.c -o P1.o -lm
mpicc main.2.c -o P1.o -lm

MIN_N=1
MAX_N=8

for i in $(seq $MIN_N $MAX_N)
do
	sbatch -J P1_$i -o ./outs/P1_$i.o -e ./outs/P1_$i.e -N 2 -n $i --mem=64GB --time=1-00:00:00 task.sh P1.o
done
