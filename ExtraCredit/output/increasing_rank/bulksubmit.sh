#!/bin/bash

SIZE_MODIFIER=20020
BATCH_SIZE=3
batch_last_job_id=""
batch_count=0

for ranks in $(seq 2 12); do
    if [[ -n "$batch_last_job_id" && $batch_count -ge $BATCH_SIZE ]]; then
        # this job won't start until the last job in the previous batch finishes
        job_id=$(sbatch --job-name=DaltonMandelbrot_r${ranks} \
                        --nodes=${ranks} \
                        --dependency=afterany:${batch_last_job_id} \
                        --export=ALL,SIZE_MODIFIER=${SIZE_MODIFIER} \
                        submit.sh | awk '{print $NF}')
        batch_count=1
    else
        job_id=$(sbatch --job-name=DaltonMandelbrot_r${ranks} \
                        --nodes=${ranks} \
                        --export=ALL,SIZE_MODIFIER=${SIZE_MODIFIER} \
                        submit.sh | awk '{print $NF}')
        batch_count=$((batch_count + 1))
    fi

    echo "Submitted job with ${ranks} ranks (job ID: ${job_id})"
    batch_last_job_id=$job_id
done

echo "Submitted jobs:"
squeue 

echo "All jobs submitted — SLURM will handle ordering"