#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --output=log/slurm-%j-gen-out.log
#SBATCH --error=log/slurm-%j-gen-err.log

N=1000000
SEED=42
KEY_SPACE=1048576

cd "$SLURM_SUBMIT_DIR"

find . -maxdepth 2 -name "slurm-*-gen-out.log" ! -name "slurm-${SLURM_JOB_ID}-gen-out.log" -delete
find . -maxdepth 2 -name "slurm-*-gen-err.log" ! -name "slurm-${SLURM_JOB_ID}-gen-err.log" -delete

make generate_datasets

echo ""
echo "---------------------------------"
echo "./generate_datasets (N=$N, SEED=$SEED, KEY_SPACE=$KEY_SPACE)"
./generate_datasets $N $N $SEED $KEY_SPACE dataset
echo "---------------------------------"
echo ""