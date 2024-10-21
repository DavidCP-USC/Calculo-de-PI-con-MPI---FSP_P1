#!/bin/bash

MAX_ITE=1000000

for i in $(seq 1 $MAX_ITE)
do
    srun $1 $i
done