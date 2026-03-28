#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --output=slurm-%j-out.log
#SBATCH --error=slurm-%j-err.log

N=1000000
P=128
seed=42
key_space=1048576

cd $SLURM_SUBMIT_DIR

# Remove old log files
find . -maxdepth 1 -name "slurm-*-out.log" ! -name "slurm-${SLURM_JOB_ID}-out.log" -delete
find . -maxdepth 1 -name "slurm-*-err.log" ! -name "slurm-${SLURM_JOB_ID}-err.log" -delete

make cleanall
make

echo ""
echo "---------------------------------"
echo "./generate_datasets"
./generate_datasets $N $N $seed $key_space dataset
echo "---------------------------------"
echo "./plain_novec"
./plain_novec $P
echo "---------------------------------"
echo "./plain_vec"
./plain_vec $P
echo "---------------------------------"
echo ""