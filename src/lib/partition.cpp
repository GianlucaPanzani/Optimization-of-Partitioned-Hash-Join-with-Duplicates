#include <stdexcept>

#include "partition.hpp"


static void partition_with_mask_hashing(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P) {
    const std::size_t n = keys.size();
    const uint64_t mask = static_cast<uint64_t>(P - 1);
    const uint64_t* in = keys.data();

    std::size_t i = 0;
    for (; i < n; ++i) {
        part_id[i] = static_cast<uint32_t>(in[i] & mask);
    }
}



static inline uint32_t mul_hash_u64_to_partition(uint64_t x, uint32_t bits) {
    constexpr uint64_t kMul = 11400714819323198485ull;
    return static_cast<uint32_t>((x * kMul) >> (64 - bits));
}

static void partition_with_mul_hashing(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P) {
    const uint32_t bits = __builtin_ctz(P);
    const std::size_t n = keys.size();
    const uint64_t* in = keys.data();
    uint32_t* out = part_id.data();

    for (std::size_t i = 0; i < n; ++i) {
        out[i] = mul_hash_u64_to_partition(in[i], bits);
    }
}



inline uint32_t fmix64(uint64_t x, uint64_t mask) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdull;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ull;
    x ^= x >> 33;
    return static_cast<uint32_t>(x & mask);
}

static void partition_with_fmix64_hashing(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P) {
    const uint64_t mask = static_cast<uint64_t>(P - 1);
    const std::size_t n = keys.size();
    const uint64_t* in = keys.data();
    uint32_t* out = part_id.data();

    for (std::size_t i = 0; i < n; ++i) {
        out[i] = fmix64(in[i], mask);
    }
}


void compute_partitions(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P, const std::string& hash_name) {
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }

    if (hash_name == "mask") partition_with_mask_hashing(keys, part_id, P);
    else if (hash_name == "mul") partition_with_mul_hashing(keys, part_id, P);
    else if (hash_name == "fmix64") partition_with_fmix64_hashing(keys, part_id, P);
    else throw std::invalid_argument("The hash function could only be: mask, mul, fmix64");
}
