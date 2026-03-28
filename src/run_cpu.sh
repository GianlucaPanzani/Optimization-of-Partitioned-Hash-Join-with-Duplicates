#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map
#SBATCH --time=00:01:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --output=out/slurm-%j-cpu.log
#SBATCH --error=err/slurm-%j-cpu.log

P=128

cd "$SLURM_SUBMIT_DIR"

find ./out -maxdepth 1 -name "slurm-*-cpu.log" ! -name "slurm-${SLURM_JOB_ID}-cpu.log" -delete
find ./err -maxdepth 1 -name "slurm-*-cpu.log" ! -name "slurm-${SLURM_JOB_ID}-cpu.log" -delete

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