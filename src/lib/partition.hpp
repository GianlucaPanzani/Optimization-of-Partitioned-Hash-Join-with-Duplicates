#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <cstdint>
#include <string>
#include <vector>

void compute_partitions(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P, const std::string& hash_name);

#endif
