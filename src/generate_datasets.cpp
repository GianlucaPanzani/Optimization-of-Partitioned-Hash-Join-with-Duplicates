#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace fs = std::filesystem;


struct Config {
    uint64_t r_size = 1'000'000;
    uint64_t s_size = 1'000'000;
    uint64_t seed = 42;
    uint64_t key_space = (1ULL << 20);
    std::string out_dir = "dataset";
};


static Config parse_args(int argc, char** argv) {
    Config cfg;
    if (argc > 1) cfg.r_size    = std::stoull(argv[1]);
    if (argc > 2) cfg.s_size    = std::stoull(argv[2]);
    if (argc > 3) cfg.seed      = std::stoull(argv[3]);
    if (argc > 4) cfg.key_space = std::stoull(argv[4]);
    if (argc > 5) cfg.out_dir   = argv[5];
    return cfg;
}


static std::vector<uint64_t> generate_keys(uint64_t n, uint64_t seed, uint64_t key_space) {
    // Error case
    if (key_space == 0) {
        throw std::invalid_argument("key_space must be > 0");
    }

    // Generate the keys from a uniform random distribution
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<uint64_t> distribution(0, key_space-1);
    std::vector<uint64_t> keys(n);
    for (uint64_t i = 0; i < n; ++i) {
        keys[i] = distribution(rng);
    }
    return keys;
}


static void write_binary_dataset(const fs::path& path, const std::vector<uint64_t>& data) {
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


static void write_metadata(const fs::path& path, const Config& cfg) {
    // Output file stream
    std::ofstream out(path);

    // Error opening the file
    if (!out) {
        throw std::runtime_error("Cannot open metadata file: " + path.string());
    }

    // Write on the output file
    out << "r_size=" << cfg.r_size << "\n";
    out << "s_size=" << cfg.s_size << "\n";
    out << "seed=" << cfg.seed << "\n";
    out << "key_space=" << cfg.key_space << "\n";
    out << "format=uint64_count + uint64_keys_binary\n";
}


// ===============================
// ============ MAIN =============
// ===============================
int main(int argc, char** argv) {
    try {
        // Get args from terminal
        const Config cfg = parse_args(argc, argv);

        // Create the ouput directory
        fs::create_directories(cfg.out_dir);

        // Generate the keys
        const std::vector<uint64_t> R = generate_keys(cfg.r_size, cfg.seed, cfg.key_space);
        const std::vector<uint64_t> S = generate_keys(cfg.s_size, cfg.seed+1, cfg.key_space);
        std::cout << "Keys generated:\n";
        std::cout << " - R size: " << cfg.r_size << " keys\n";
        std::cout << " - S size: " << cfg.s_size << " keys\n";
        std::cout << " - Seed for R: " << cfg.seed << "\n";
        std::cout << " - Seed for S: " << (cfg.seed+1) << "\n";
        std::cout << " - Key space: " << cfg.key_space << "\n";
        
        // Create the datasets in binary files
        std::string R_filename = "R.bin";
        std::string S_filename = "S.bin";
        write_binary_dataset(fs::path(cfg.out_dir) / R_filename, R);
        write_binary_dataset(fs::path(cfg.out_dir) / S_filename, S);
        std::cout << "Datasets generated: " << (fs::path(cfg.out_dir) / R_filename) << ", " << (fs::path(cfg.out_dir) / S_filename) << "\n";

        // Create the metadata file
        std::string metadata_filename = "metadata.txt";
        write_metadata(fs::path(cfg.out_dir) / metadata_filename, cfg);
        std::cout << "Metadata file generated: " << (fs::path(cfg.out_dir) / metadata_filename) << "\n\n";
        
        std::cout << "Execution ended correctly!\n";
        std::cout << "Files generated:\n";
        std::cout << "  " << (fs::path(cfg.out_dir) / R_filename) << "\n";
        std::cout << "  " << (fs::path(cfg.out_dir) / S_filename) << "\n";
        std::cout << "  " << (fs::path(cfg.out_dir) / metadata_filename) << "\n";
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}