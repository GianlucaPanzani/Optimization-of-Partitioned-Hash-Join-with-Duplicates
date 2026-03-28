#ifndef PARTITION_AVX2_HPP
#define PARTITION_AVX2_HPP

#include <cstdint>
#include <vector>

std::vector<uint32_t> compute_partitions_avx2(const std::vector<uint64_t>& keys, uint32_t P);

#endif