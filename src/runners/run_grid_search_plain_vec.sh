#!/bin/bash
#SBATCH --job-name=gpanzani_partition_map_plain_vec
#SBATCH --time=00:05:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --partition=normal
#SBATCH --output=out/slurm-%j-plain_vec.log
#SBATCH --error=err/slurm-%j-plain_vec.log

set -e

cd "$SLURM_SUBMIT_DIR"
source "$SLURM_SUBMIT_DIR/grid_config.sh"

TOTAL=$(( ${#N_VALUES[@]} * ${#P_VALUES[@]} * ${#HASH_VALUES[@]} ))
COUNT=0

for N in "${N_VALUES[@]}"; do
    for P in "${P_VALUES[@]}"; do
        for HASH in "${HASH_VALUES[@]}"; do
            COUNT=$((COUNT + 1))

            echo -n "[$COUNT/$TOTAL] Executing combination N=$N P=$P HASH=$HASH ..."
            bash run_plain_vec.sh "$N" "$P" "$HASH"

        done
    done
done

echo ""
echo "All jobs submitted."