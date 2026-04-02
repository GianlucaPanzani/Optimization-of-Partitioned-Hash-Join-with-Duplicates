#include <string>
#include <stdexcept>
#include <immintrin.h>
#include "partition_avx2.hpp"


std::vector<uint32_t> compute_partitions_avx2(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P, const std::string& hash_name) {
    // P validation check
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }
    const uint64_t P_mask = static_cast<uint64_t>(P - 1);

    // Build the mask vector to be applied to 4 progressive uint64_t keys
    const __m256i mask_vec = _mm256_set1_epi64x(static_cast<long long>(P_mask));

    // For each step are processed 4 uint64_t keys (= 1 lane)
    std::size_t i = 0;
    const std::size_t simd_width = 4; // because AVX2 uses registers of 256 bits (256 / 64 = 4)
    for (; i + simd_width <= keys.size(); i += simd_width) {
        // Build the vector of 4 progressive uint64_t keys (from i to i+3)
        __m256i key_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&keys[i]));

        // Bitwise AND with mask
        __m256i part_vec = _mm256_and_si256(key_vec, mask_vec);

        // Store temporary 64-bit results
        alignas(32) uint64_t tmp[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), part_vec);

        // Cast the current lane to uint32_t
        part_id[i+0] = static_cast<uint32_t>(tmp[0]);
        part_id[i+1] = static_cast<uint32_t>(tmp[1]);
        part_id[i+2] = static_cast<uint32_t>(tmp[2]);
        part_id[i+3] = static_cast<uint32_t>(tmp[3]);
    }
    
    // Cycle done for the last possible positions remained (< 4)
    for (; i < keys.size(); ++i) {
        part_id[i] = static_cast<uint32_t>(keys[i] & P_mask);
    }

    return part_id;
}