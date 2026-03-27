#ifndef DATASET_HPP
#define DATASET_HPP

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

struct Dataset {
    uint64_t size;
    std::vector<uint64_t> keys;
};

Dataset load_dataset(const std::string& path);
void write_binary_dataset(const std::filesystem::path& path, const std::vector<uint64_t>& data);

#endif