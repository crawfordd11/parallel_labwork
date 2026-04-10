#!/bin/bash
#SBATCH --job-name=DaltonMandelbrot
#SBATCH --partition=batch
#SBATCH --nodes=9
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=80
#SBATCH --time=00:01:20
#SBATCH --output=output_image_program.%j
#SBATCH --export=ALL

SIZE_MODIFIER=${SIZE_MODIFIER:-20020}

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

srun ./mandelbrot.cx $SIZE_MODIFIER
