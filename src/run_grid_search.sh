#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

show_vectorization_report() {
    local title="$1"
    local report_file="$2"
    local source_regex="$3"

    echo "$title"
    if [ ! -s "$report_file" ]; then
        echo "(no vectorization diagnostics written to $report_file)"
        return
    fi

    if ! grep -E "^(${source_regex}):.*(optimized: loop vectorized|missed: couldn't vectorize|missed: not vectorized: no vectype)" "$report_file"; then
        echo "(no relevant vectorization lines found in $report_file)"
    fi
}

mkdir -p results
rm -f results/results_tmp.csv
if [ -f results/results.csv ]; then
    mv results/results.csv results/results_tmp.csv
fi

echo ">>> Cleaning previous results and logs <<<"
make cleanall

echo ">>> Compiling files <<<"
make

show_vectorization_report \
    ">>> GCC vectorization report for plain_vec (lib/partition.cpp) <<<" \
    "results/gcc_vec_report.txt" \
    "lib/partition\\.cpp"

echo ">>> GCC vectorization report for plain_novec <<<"
echo "Vectorization is disabled with -fno-tree-vectorize, so results/gcc_novec_report.txt is expected to be empty."

show_vectorization_report \
    ">>> GCC vectorization report for avx2 (lib/partition_avx2.cpp) <<<" \
    "results/gcc_avx2_report.txt" \
    "lib/partition_avx2\\.cpp"

echo ">>> Generating datasets <<<"
GEN_JOB_ID=$(sbatch --parsable runners/run_gen_dataset.sh)
echo "runners/run_gen_dataset.sh -> job $GEN_JOB_ID"

echo ">>> Executing grid search with NO vectorization <<<"
PLAIN_NOVEC_JOB_ID=$(sbatch --parsable --dependency=afterok:${GEN_JOB_ID} runners/run_grid_search_plain_novec.sh)
echo "runners/run_grid_search_plain_novec.sh -> job $PLAIN_NOVEC_JOB_ID"

echo ">>> Executing grid search with vectorization <<<"
PLAIN_VEC_JOB_ID=$(sbatch --parsable --dependency=afterok:${PLAIN_NOVEC_JOB_ID} runners/run_grid_search_plain_vec.sh)
echo "runners/run_grid_search_plain_vec.sh -> job $PLAIN_VEC_JOB_ID"

echo ">>> Executing grid search with AVX2 <<<"
AVX2_GRID_SEARCH_JOB_ID=$(sbatch --parsable --dependency=afterok:${PLAIN_VEC_JOB_ID} runners/run_grid_search_avx2.sh)
echo "runners/run_grid_search_avx2.sh -> job $AVX2_GRID_SEARCH_JOB_ID"

echo ">>> Results will be available in: results/results.csv <<<"
