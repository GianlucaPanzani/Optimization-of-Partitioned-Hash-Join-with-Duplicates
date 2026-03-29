#ifndef PARTITION_HPP
#define PARTITION_HPP

#include <cstdint>
#include <string>
#include <vector>

std::vector<uint32_t> hash_function1(const std::vector<uint64_t>& keys, uint32_t P);
std::vector<uint32_t> hash_function2(const std::vector<uint64_t>& keys, uint32_t P);
std::vector<uint32_t> hash_function3(const std::vector<uint64_t>& keys, uint32_t P);
std::vector<uint32_t> compute_partitions(const std::vector<uint64_t>& keys, uint32_t P, std::string hash_name);

#endif
