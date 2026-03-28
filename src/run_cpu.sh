#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --output=log/slurm-%j-cpu-out.log
#SBATCH --error=log/slurm-%j-cpu-err.log

P=128

cd "$SLURM_SUBMIT_DIR"

find . -maxdepth 2 -name "slurm-*-cpu-out.log" ! -name "slurm-${SLURM_JOB_ID}-cpu-out.log" -delete
find . -maxdepth 2 -name "slurm-*-cpu-err.log" ! -name "slurm-${SLURM_JOB_ID}-cpu-err.log" -delete

make plain_vec
make plain_novec

echo ""
echo "---------------------------------"
echo "./plain_novec (P=$P)"
./plain_novec $P
echo "---------------------------------"
echo "./plain_vec (P=$P)"
./plain_vec $P
echo "---------------------------------"
echo ""