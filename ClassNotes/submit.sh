#!/bin/bash
#SBATCH --job-name=dot_scatter.c
#SBATCH --partition=batch
#SBATCH --nodes=7
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:00:10
#SBATCH --output=op_dot_scatter.%j
#SBATCH --export=ALL

srun ./dot_scatterv.x
