#include <string>
#include "partition_avx2.hpp"
#include "partition.hpp"


std::vector<uint32_t> compute_partitions_avx2_mask(const std::vector<uint64_t>& keys, uint32_t P) {
    return compute_partitions(keys, P, "mask");
}


std::vector<uint32_t> compute_partitions_avx2(const std::vector<uint64_t>& keys, uint32_t P, const std::string& hash_name) {
    return compute_partitions(keys, P, hash_name);
}
