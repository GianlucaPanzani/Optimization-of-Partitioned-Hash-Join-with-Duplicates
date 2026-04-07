#ifndef PARTITION_AVX2_HPP
#define PARTITION_AVX2_HPP

#include <cstdint>
#include <string>
#include <vector>

void partition_with_mask_hashing_avx2(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P);

#endif
