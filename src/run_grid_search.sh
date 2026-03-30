#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

mkdir -p results
rm -f results/results_tmp.csv
if [ -f results/results.csv ]; then
    mv results/results.csv results/results_tmp.csv
fi

echo ">>> Cleaning previous results and logs <<<"
make cleanall

echo ">>> Compiling files <<<"
make

echo ">>> Optimizations got by compiling with vectorization:"
grep optimized results/gcc_vec_report.txt | grep "lib/"

echo ">>> Optimizations got by compiling with NO vectorization:"
grep optimized results/gcc_novec_report.txt | grep "lib/"

echo ">>> Generating datasets <<<"
GEN_JOB_ID=$(sbatch --parsable runners/run_gen_dataset.sh)
echo "runners/run_gen_dataset.sh -> job $GEN_JOB_ID"

echo ">>> Executing grid search with vectorization <<<"
PLAIN_VEC_JOB_ID=$(sbatch --parsable --dependency=afterok:${GEN_JOB_ID} runners/run_grid_search_plain_vec.sh)
echo "runners/run_grid_search_plain_vec.sh -> job $PLAIN_VEC_JOB_ID"

echo ">>> Executing grid search with NO vectorization <<<"
PLAIN_NOVEC_JOB_ID=$(sbatch --parsable --dependency=afterok:${PLAIN_VEC_JOB_ID} runners/run_grid_search_plain_novec.sh)
echo "runners/run_grid_search_plain_novec.sh -> job $PLAIN_NOVEC_JOB_ID"

#echo ">>> Executing grid search with AVX2 <<<"
#GPU_GRID_SEARCH_JOB_ID=$(sbatch --parsable --dependency=afterok:${PLAIN_NOVEC_JOB_ID} runners/run_grid_search_avx2.sh)
#echo "runners/run_grid_search_avx2.sh -> job $GPU_GRID_SEARCH_JOB_ID"

echo ">>> Results will be available in: results/results.csv"
