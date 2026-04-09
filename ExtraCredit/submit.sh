#!/bin/bash
#SBATCH --job-name=LeonMandelbrot
#SBATCH --partition=batch
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=16
#SBATCH --time=00:00:20
#SBATCH --output=output_program.%j
#SBATCH --export=ALL

SIZE_MODIFIER=20020

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

srun ./mandelbrot.cx $SIZE_MODIFIER