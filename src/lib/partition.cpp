#include <stdexcept>
#include "partition.hpp"


std::vector<uint32_t> compute_partitions(const std::vector<uint64_t>& keys, uint32_t P) {
    // P validation check
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }

    // Loop for the assigning of the partiotions (to every key)
    std::vector<uint32_t> part_id(keys.size());
    const uint64_t mask = P-1;
    for (size_t i = 0; i < keys.size(); ++i) {
        uint64_t value_in_0_P = keys[i] & mask; // logic AND between corresponding binary values
        part_id[i] = static_cast<uint32_t>(value_in_0_P); // casting to uint32_t
    }

    return part_id;
}