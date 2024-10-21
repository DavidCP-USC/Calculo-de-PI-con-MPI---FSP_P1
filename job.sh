#!/bin/bash

rm -r ./outs/*

module load gcc openmpi/4.1.4_ft3

mpicc main.c -o FSP_P1_V1.o -lm

MAX_N=64

for i in $(seq 1 $MAX_N)
do
    sbatch -J FSP_P1_$i -o ./outs/FSP_P1_$i.o -e ./outs/FSP_P1_$i.e -N 2 -n $i --mem=32GB --time=01:55:00 task.sh FSP_P1_V1.o
done
