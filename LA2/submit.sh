#!/bin/bash
#SBATCH --job-name=program
#SBATCH --partition=debug
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:00:10
#SBATCH --output=output_program.%j
#SBATCH --export=ALL

srun ./LA2.x
