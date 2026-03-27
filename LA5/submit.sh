#!/bin/bash
#SBATCH --job-name=program
#SBATCH --partition=batch
#SBATCH --exclude=sc3
#SBATCH --nodes=10
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:01:00
#SBATCH --output=output_program.%j
#SBATCH --export=ALL

srun ./LA5.x
