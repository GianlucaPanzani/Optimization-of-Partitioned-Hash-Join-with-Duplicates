#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=gpu-shared
#SBATCH --output=log/slurm-%j-gpu-out.log
#SBATCH --error=log/slurm-%j-gpu-err.log

P=128

cd "$SLURM_SUBMIT_DIR"

find . -maxdepth 2 -name "slurm-*-gpu-out.log" ! -name "slurm-${SLURM_JOB_ID}-gpu-out.log" -delete
find . -maxdepth 2 -name "slurm-*-gpu-err.log" ! -name "slurm-${SLURM_JOB_ID}-gpu-err.log" -delete

make avx2

echo ""
echo "---------------------------------"
echo "./avx2 (P=$P)"
./avx2 $P
echo "---------------------------------"
echo ""