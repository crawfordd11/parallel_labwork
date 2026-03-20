#!/bin/bash
#SBATCH --job-name=LA4Convolution51
#SBATCH --partition=batch
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:00:30
#SBATCH --output=out_simpleBoxBlur.%j
#SBATCH --export=ALL

srun ./LA4SimpleBoxBlur.x
