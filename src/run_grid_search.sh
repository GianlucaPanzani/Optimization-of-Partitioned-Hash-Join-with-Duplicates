#!/bin/bash

echo ">>> Cleaning previous results and logs ..."
make cleanall

echo ">>> Compiling files ..."
make

echo ">>> Optimizations got by compiling with vectorization:"
grep optimized results/gcc_vec_report.txt

echo ">>> Optimizations got by compiling with NO vectorization:"
grep optimized results/gcc_novec_report.txt

echo ">>> Generating datasets ..."
GEN_JOB_ID=$(sbatch --parsable runners/generate_datasets.sh)
echo "runners/generate_datasets.sh -> job $GEN_JOB_ID"

echo ">>> Executing grid search with vectorization ..."
CPU_GRID_SEARCH_JOB_ID=$(sbatch --parsable --dependency=afterok:${GEN_JOB_ID} runners/run_grid_search_plain_vec.sh)
echo "runners/run_grid_search_plain_vec.sh -> job $CPU_GRID_SEARCH_JOB_ID"

echo ">>> Executing grid search with NO vectorization ..."
CPU_GRID_SEARCH_JOB_ID=$(sbatch --parsable --dependency=afterok:${GEN_JOB_ID} runners/run_grid_search_plain_novec.sh)
echo "runners/run_grid_search_plain_novec.sh -> job $CPU_GRID_SEARCH_JOB_ID"

#echo ">>> Executing grid search with AVX2 ..."
#GPU_GRID_SEARCH_JOB_ID=$(sbatch --parsable --dependency=afterok:${CPU_GRID_SEARCH_JOB_ID} runners/run_grid_search_avx2.sh)
#echo "runners/run_grid_search_avx2.sh -> job $GPU_GRID_SEARCH_JOB_ID"

echo ">>> Results will be available in: results/results_json.json"