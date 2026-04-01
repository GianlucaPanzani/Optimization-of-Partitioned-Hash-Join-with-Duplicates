#!/bin/bash

if [ "$#" -lt 3 ] || [ "$#" -gt 4 ]; then
    echo
    echo "Usage: $0 [N] [P] [HASH] [OUTPUT_CSV(optional)]"
    echo "  N: number of keys to be used --> [100000, 1000000, 10000000, 50000000, 100000000],"
    echo "  P: number of partitions --> [64, 128, 256, 512, 1024, 2048, 4096],"
    echo "  HASH: hash function to be used --> [mask, mul, fmix64]."
    exit 1
fi

N=$1
P=$2
HASH=$3
OUTPUT_CSV=${4:-}

CMD=(./plain_novec "$N" "$P" "$HASH" plain_novec)
if [ -n "$OUTPUT_CSV" ]; then
    CMD+=("$OUTPUT_CSV")
fi

"${CMD[@]}"
