#!/bin/bash

SIZE_MODIFIER=20020

for ranks in $(seq 1 12); do
    sbatch --job-name=DaltonMandelbrot_r${ranks} \
           --nodes=${ranks} \
           --export=ALL,SIZE_MODIFIER=${SIZE_MODIFIER} \
           submit.sh
    echo "Submitted job with ${ranks} ranks"
done