#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>


struct Dataset {
    uint64_t size;
    std::vector<uint64_t> keys;
};


struct Config {
    uint64_t P = 128;
};


static Config parse_args(int argc, char** argv) {
    Config cfg;
    if (argc > 1) cfg.P    = std::stoull(argv[1]);
    return cfg;
}


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


std::vector<uint32_t> compute_partitions(const std::vector<uint64_t>& keys, uint64_t P) {
    if (P == 0) {
        throw std::invalid_argument("P must be > 0");
    }

    if ((P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }

    std::vector<uint32_t> part_id(keys.size());
    const uint64_t mask = P - 1;

    for (size_t i = 0; i < keys.size(); ++i) {
        part_id[i] = static_cast<uint32_t>(keys[i] & mask);
    }

    return part_id;
}



int main(int argc, char** argv) {
    const Config cfg = parse_args(argc, argv);
    const std::uint64_t P = cfg.P;

    // Load datasets
    Dataset R = load_dataset("dataset/R.bin");
    Dataset S = load_dataset("dataset/S.bin");

    // Compute the partitions for each dataset
    std::vector<std::uint32_t> R_partitions = compute_partitions(R.keys, P);
    std::vector<std::uint32_t> S_partitions = compute_partitions(S.keys, P);

    return 0;
}
