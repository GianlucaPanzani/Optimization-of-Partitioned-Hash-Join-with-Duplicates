#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo
    echo "Usage: $0 [N] [P] [HASH]"
    echo "  N: number of keys to be used --> [100000, 1000000, 10000000, 50000000, 100000000],"
    echo "  P: number of partitions --> [64, 128, 256, 512, 1024, 2048, 4096],"
    echo "  HASH: hash function to be used --> [mask, mul, fmix64]."
    exit 1
fi

N=$1
P=$2
HASH=$3

./plain_novec "$N" "$P" "$HASH" plain_novec
