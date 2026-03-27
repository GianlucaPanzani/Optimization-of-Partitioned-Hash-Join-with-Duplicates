#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "dataset.hpp"



Dataset load_dataset(const std::string& path) {
    // Input file stream
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open dataset file: " + path);
    }

    // Read the size of the dataset (first element stored in the file)
    Dataset ds{};
    in.read(reinterpret_cast<char*>(&ds.size), sizeof(ds.size));
    if (!in) {
        throw std::runtime_error("Failed to read dataset size from: " + path);
    }

    // Read the data/keys
    ds.keys.resize(ds.size); // set the size of the vector as the size read before (size = number of uint64_t)
    in.read(reinterpret_cast<char*>(ds.keys.data()), static_cast<std::streamsize>(ds.size * sizeof(uint64_t)));
    if (!in) {
        throw std::runtime_error("Failed to read dataset payload from: " + path);
    }

    return ds;
}


void write_binary_dataset(const std::filesystem::path& path, const std::vector<uint64_t>& data) {
    // Output file stream
    std::ofstream out(path, std::ios::binary);

    // Error opening the file
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + path.string());
    }
    
    // Write on the output file
    const uint64_t n = static_cast<uint64_t>(data.size());
    out.write(reinterpret_cast<const char*>(&n), sizeof(n));
    out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(n * sizeof(uint64_t)));

    // Error during writing
    if (!out) {
        throw std::runtime_error("Error while writing file: " + path.string());
    }
}