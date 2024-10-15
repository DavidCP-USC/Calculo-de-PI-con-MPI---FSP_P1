#!/bin/bash

#SBATCH -J FSP_P1
#SBATCH -o FSP_P1_%j.o
#SBATCH -e FSP_P1_%j.e
#SBATCH -N 2
#SBATCH -n 20
#SBATCH -t 01:30:00
#SBATCH --mem=1G

module load gcc openmpi/4.1.4_ft3

srun $1
