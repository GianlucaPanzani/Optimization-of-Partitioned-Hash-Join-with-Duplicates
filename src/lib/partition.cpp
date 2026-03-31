#include <stdexcept>

#include "partition.hpp"


std::vector<uint32_t> partition_with_mask_hashing(const std::vector<uint64_t>& keys, uint32_t P) {
    std::vector<uint32_t> part_id(keys.size());
    const uint64_t mask = static_cast<uint64_t>(P - 1);

    for (size_t i = 0; i < keys.size(); ++i) {
        part_id[i] = static_cast<uint32_t>(keys[i] & mask); // bitwise AND between corresponding binary values
    }

    return part_id;
}


std::vector<uint32_t> partition_with_mul_hashing(const std::vector<uint64_t>& keys, uint32_t P) {
    std::vector<uint32_t> part_id(keys.size());
    const uint32_t bits = __builtin_ctz(P);
    constexpr uint64_t kMul = 11400714819323198485ull; // Fibonacci hashing constant

    for (size_t i = 0; i < keys.size(); ++i) {
        part_id[i] = static_cast<uint32_t>((keys[i] * kMul) >> (64 - bits));
    }

    return part_id;
}


std::vector<uint32_t> partition_with_fmix64_hashing(const std::vector<uint64_t>& keys, uint32_t P) {
    std::vector<uint32_t> part_id(keys.size());
    const uint64_t mask = static_cast<uint64_t>(P - 1);

    for (size_t i = 0; i < keys.size(); ++i) {
        uint64_t x = keys[i];
        x ^= x >> 33;
        x *= 0xff51afd7ed558ccdull;
        x ^= x >> 33;
        x *= 0xc4ceb9fe1a85ec53ull;
        x ^= x >> 33;
        part_id[i] = static_cast<uint32_t>(x & mask);
    }

    return part_id;
}



std::vector<uint32_t> compute_partitions(const std::vector<uint64_t>& keys, uint32_t P, const std::string& hash_name) {
    // P validation check
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }

    std::vector<uint32_t> part_id;
    if (hash_name == "mask") part_id = partition_with_mask_hashing(keys, P);
    else if (hash_name == "mul") part_id = partition_with_mul_hashing(keys, P);
    else if (hash_name == "fmix64") part_id = partition_with_fmix64_hashing(keys, P);
    else throw std::invalid_argument("The hash function could only be: mask, mul, fmix64");

    return part_id;
}
