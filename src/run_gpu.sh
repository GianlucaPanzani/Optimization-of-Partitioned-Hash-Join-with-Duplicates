#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=gpu-shared
#SBATCH --output=out/slurm-%j-gpu.log
#SBATCH --error=err/slurm-%j-gpu.log

P=128

cd "$SLURM_SUBMIT_DIR"

find ./out -maxdepth 1 -name "slurm-*-gpu.log" ! -name "slurm-${SLURM_JOB_ID}-gpu.log" -delete
find ./err -maxdepth 1 -name "slurm-*-gpu.log" ! -name "slurm-${SLURM_JOB_ID}-gpu.log" -delete

make avx2

echo ""
echo "---------------------------------"
echo "./avx2 (P=$P)"
./avx2 $P
echo "---------------------------------"
echo ""