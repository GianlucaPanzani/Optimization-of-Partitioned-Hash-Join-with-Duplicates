#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map_avx2
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=gpu-shared
#SBATCH --output=out/slurm-%j-avx2.log
#SBATCH --error=err/slurm-%j-avx2.log

set -e

cd "$SLURM_SUBMIT_DIR"
source "$SLURM_SUBMIT_DIR/runners/grid_config.sh"

TOTAL=$(( ${#N_VALUES[@]} * ${#P_VALUES[@]} * ${#HASH_VALUES[@]} ))
COUNT=0

for N in "${N_VALUES[@]}"; do
    for P in "${P_VALUES[@]}"; do
        for HASH in "${HASH_VALUES[@]}"; do
            COUNT=$((COUNT + 1))

            echo -n "[$COUNT/$TOTAL] N=$N P=$P HASH=$HASH"
            bash "$SLURM_SUBMIT_DIR/runners/run_avx2.sh" "$N" "$P" "$HASH"

        done
    done
done

echo ""
echo "All jobs submitted."
