#!/bin/bash
#SBATCH --job-name=LeonMandelbrot
#SBATCH --partition=batch
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:00:20
#SBATCH --output=output_program.%j
#SBATCH --export=ALL

srun ./mandelbrot.cx
