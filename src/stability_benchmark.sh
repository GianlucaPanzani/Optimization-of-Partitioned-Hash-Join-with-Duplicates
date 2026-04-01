#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"
source "$SCRIPT_DIR/runners/grid_benchmark_config.sh"

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Usage: $0 [M] [OUTPUT_CSV(optional)]"
    echo "  M: number of repetitions for each configuration in $GRID_CONFIG_FILE."
    exit 1
fi

M=$1
OUTPUT_CSV=${2:-$DEFAULT_OUTPUT_CSV}

if ! [[ "$M" =~ ^[1-9][0-9]*$ ]]; then
    echo "M must be a positive integer, received: $M"
    exit 1
fi

build_backup_path() {
    local output_path="$1"
    local directory filename basename extension

    directory=$(dirname "$output_path")
    filename=$(basename "$output_path")

    if [[ "$filename" == *.* ]]; then
        basename="${filename%.*}"
        extension=".${filename##*.}"
    else
        basename="$filename"
        extension=""
    fi

    echo "$directory/${basename}_tmp${extension}"
}

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

mkdir -p "$(dirname "$OUTPUT_CSV")"
BACKUP_RESULTS_FILE=$(build_backup_path "$OUTPUT_CSV")
rm -f "$BACKUP_RESULTS_FILE"
if [ -f "$OUTPUT_CSV" ]; then
    mv "$OUTPUT_CSV" "$BACKUP_RESULTS_FILE"
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

echo ">>> Executing repeated grid search with NO vectorization <<<"
PLAIN_NOVEC_JOB_ID=$(sbatch --parsable --dependency=afterok:${GEN_JOB_ID} runners/run_grid_search_plain_novec.sh "$OUTPUT_CSV" "$GRID_CONFIG_FILE" "$M")
echo "runners/run_grid_search_plain_novec.sh -> job $PLAIN_NOVEC_JOB_ID"

echo ">>> Executing repeated grid search with vectorization <<<"
PLAIN_VEC_JOB_ID=$(sbatch --parsable --dependency=afterok:${PLAIN_NOVEC_JOB_ID} runners/run_grid_search_plain_vec.sh "$OUTPUT_CSV" "$GRID_CONFIG_FILE" "$M")
echo "runners/run_grid_search_plain_vec.sh -> job $PLAIN_VEC_JOB_ID"

echo ">>> Executing repeated grid search with AVX2 <<<"
AVX2_GRID_SEARCH_JOB_ID=$(sbatch --parsable --dependency=afterok:${PLAIN_VEC_JOB_ID} runners/run_grid_search_avx2.sh "$OUTPUT_CSV" "$GRID_CONFIG_FILE" "$M")
echo "runners/run_grid_search_avx2.sh -> job $AVX2_GRID_SEARCH_JOB_ID"

echo ">>> Results will be available in: $OUTPUT_CSV <<<"
