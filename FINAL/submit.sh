#!/bin/bash
#SBATCH --job-name=test        # Job name
#SBATCH --output=output.txt           # Standard output file
#SBATCH --partition=batch # Partition or queue name
#SBATCH --nodes=1                    # Number of nodes
#SBATCH --ntasks-per-node=1           # Number of tasks per node
#SBATCH --time=0:00:60                # Maximum runtime (D-HH:MM:SS)



srun ./final.x
