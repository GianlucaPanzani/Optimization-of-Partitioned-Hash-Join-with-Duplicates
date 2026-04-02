#include <string>
#include <stdexcept>
#include <immintrin.h>
#include "partition_avx2.hpp"


void compute_partitions_avx2(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P, const std::string& hash_name) {
    // P validation check
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }

    // Build the mask vector to be applied to 4 progressive uint64_t keys
    const uint64_t P_mask = static_cast<uint64_t>(P - 1);
    const __m256i mask_vec = _mm256_set1_epi64x(static_cast<long long>(P_mask));

    // Select 32-bit lanes 0,2,4,6 = low 32 bits of each 64-bit lane
    const __m256i perm_idx = _mm256_setr_epi32(0, 2, 4, 6, 0, 0, 0, 0);

    const std::size_t n_keys = keys.size();
    const uint64_t* in = keys.data();
    uint32_t* out = part_id.data();

    // For each step are processed 4 uint64_t keys (= 1 lane)
    std::size_t i = 0;
    const std::size_t simd_width = 4; // because AVX2 uses registers of 256 bits (256 / 64 = 4)
    for (; i + simd_width <= n_keys; i += simd_width) {
        // Build the vector of 4 progressive uint64_t keys (from i to i+3)
        const __m256i key_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + i));

        const __m256i part_vec = _mm256_and_si256(key_vec, mask_vec);

        // Move low 32 bits of each 64-bit lane into the low 128 bits
        const __m256i packed32 = _mm256_permutevar8x32_epi32(part_vec, perm_idx);

        // Store 4 x uint32_t results
        _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i), _mm256_castsi256_si128(packed32));
    }
    
    // Cycle done for the last possible positions remained (< 4)
    for (; i < n_keys; ++i) {
        out[i] = static_cast<uint32_t>(in[i] & P_mask);
    }

}
