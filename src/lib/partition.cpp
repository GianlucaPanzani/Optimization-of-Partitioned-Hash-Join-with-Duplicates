#include <stdexcept>

#include "partition.hpp"


std::vector<uint32_t> hash_function1(const std::vector<uint64_t>& keys, uint32_t P) {
    std::vector<uint32_t> part_id(keys.size());
    const uint64_t mask = P-1;
    for (size_t i = 0; i < keys.size(); ++i) {
        uint64_t value_in_0_P = keys[i] & mask; // logic AND between corresponding binary values
        part_id[i] = static_cast<uint32_t>(value_in_0_P); // casting to uint32_t
    }
    return part_id;
}

std::vector<uint32_t> hash_function2(const std::vector<uint64_t>& keys, uint32_t P) {
    std::vector<uint32_t> part_id(keys.size());
    const uint64_t mask = P-1;
    for (size_t i = 0; i < keys.size(); ++i) {
        uint64_t value_in_0_P = keys[i] & mask; // logic AND between corresponding binary values
        part_id[i] = static_cast<uint32_t>(value_in_0_P); // casting to uint32_t
    }
    return part_id;
}

std::vector<uint32_t> hash_function3(const std::vector<uint64_t>& keys, uint32_t P) {
    std::vector<uint32_t> part_id(keys.size());
    const uint64_t mask = P-1;
    for (size_t i = 0; i < keys.size(); ++i) {
        uint64_t value_in_0_P = keys[i] & mask; // logic AND between corresponding binary values
        part_id[i] = static_cast<uint32_t>(value_in_0_P); // casting to uint32_t
    }
    return part_id;
}


std::vector<uint32_t> compute_partitions(const std::vector<uint64_t>& keys, uint32_t P, std::string hash_name) {
    // P validation check
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }

    std::vector<uint32_t> part_id;
    if (hash_name == "h1") part_id = hash_function1(keys, P);
    if (hash_name == "h2") part_id = hash_function2(keys, P);
    if (hash_name == "h3") part_id = hash_function3(keys, P);
    else throw std::invalid_argument("The hash function could only be: h1, h2, h3");
    

    return part_id;
}
