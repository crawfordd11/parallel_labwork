#!/bin/bash
#SBATCH --job-name=LA3bcastTest
#SBATCH --partition=batch
#SBATCH --nodes=10
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:00:10
#SBATCH --output=output_program.%j
#SBATCH --export=ALL

srun ./LA3.x
