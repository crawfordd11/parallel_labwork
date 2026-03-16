#!/bin/bash
#SBATCH --job-name=LA4Convolution51
#SBATCH --partition=batch
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:00:10
#SBATCH --output=out_LA4.%j
#SBATCH --export=ALL

srun ./LA4.x
squeue