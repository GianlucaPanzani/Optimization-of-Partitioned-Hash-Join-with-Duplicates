#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --output=out/slurm-%j-gen.log
#SBATCH --error=err/slurm-%j-gen.log

N=1000000
SEED=42
KEY_SPACE=1048576

cd "$SLURM_SUBMIT_DIR"

find ./out -maxdepth 1 -name "slurm-*-gen.log" ! -name "slurm-${SLURM_JOB_ID}-gen.log" -delete
find ./err -maxdepth 1 -name "slurm-*-gen.log" ! -name "slurm-${SLURM_JOB_ID}-gen.log" -delete

make generate_datasets

echo ""
echo "---------------------------------"
echo "./generate_datasets (N=$N, SEED=$SEED, KEY_SPACE=$KEY_SPACE)"
./generate_datasets $N $N $SEED $KEY_SPACE dataset
echo "---------------------------------"
echo ""