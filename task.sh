#!/bin/bash

MAX_ITE=1000000000000

i=1
while [ $i -le $MAX_ITE ]
do
	srun $1 $i
	i=$(($i * 10))
done
