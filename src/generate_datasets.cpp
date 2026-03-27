#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "lib/dataset.hpp"
#include "lib/timing.hpp"


struct Args {
    uint64_t r_size = 1'000'000;
    uint64_t s_size = 1'000'000;
    uint64_t seed = 42;
    uint64_t key_space = (1ULL << 20);
    std::string out_dir = "dataset";
};


static Args parse_args(int argc, char** argv) {
    Args args;
    if (argc > 1) args.r_size    = std::stoull(argv[1]);
    if (argc > 2) args.s_size    = std::stoull(argv[2]);
    if (argc > 3) args.seed      = std::stoull(argv[3]);
    if (argc > 4) args.key_space = std::stoull(argv[4]);
    if (argc > 5) args.out_dir   = argv[5];
    return args;
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


static void write_metadata(const std::filesystem::path& path, const Args& args) {
    // Output file stream
    std::ofstream out(path);

    // Error opening the file
    if (!out) {
        throw std::runtime_error("Cannot open metadata file: " + path.string());
    }

    // Write on the output file
    out << "r_size=" << args.r_size << "\n";
    out << "s_size=" << args.s_size << "\n";
    out << "seed=" << args.seed << "\n";
    out << "key_space=" << args.key_space << "\n";
    out << "format=uint64_count + uint64_keys_binary\n";
}


// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    // Timing variables
    double t0, t1;

    try {
        // Pick starting time
        t0 = get_time();

        // Get args from terminal
        const Args args = parse_args(argc, argv);

        // Create the ouput directory
        std::filesystem::create_directories(args.out_dir);

        // Generate the keys
        const std::vector<uint64_t> R = generate_keys(args.r_size, args.seed, args.key_space);
        const std::vector<uint64_t> S = generate_keys(args.s_size, args.seed+1, args.key_space);
        std::cout << "Keys generated:\n";
        std::cout << " - R size: " << args.r_size << " keys\n";
        std::cout << " - S size: " << args.s_size << " keys\n";
        std::cout << " - Seed for R: " << args.seed << "\n";
        std::cout << " - Seed for S: " << (args.seed+1) << "\n";
        std::cout << " - Key space: " << args.key_space << "\n";
        
        // Create the datasets in binary files
        std::string R_filename = "R.bin";
        std::string S_filename = "S.bin";
        write_binary_dataset(std::filesystem::path(args.out_dir) / R_filename, R);
        write_binary_dataset(std::filesystem::path(args.out_dir) / S_filename, S);
        std::cout << "Datasets generated: " << (std::filesystem::path(args.out_dir) / R_filename) << ", " << (std::filesystem::path(args.out_dir) / S_filename) << "\n";

        // Create the metadata file
        std::string metadata_filename = "metadata.txt";
        write_metadata(std::filesystem::path(args.out_dir) / metadata_filename, args);
        std::cout << "Metadata file generated: " << (std::filesystem::path(args.out_dir) / metadata_filename) << "\n";
        
        // Pick ending time
        t1 = get_time();
        std::cout << "Execution ended correctly after " << get_diff(t0, t1, 5) << " s\n";
        std::cout << "Files generated:\n";
        std::cout << "  " << (std::filesystem::path(args.out_dir) / R_filename) << "\n";
        std::cout << "  " << (std::filesystem::path(args.out_dir) / S_filename) << "\n";
        std::cout << "  " << (std::filesystem::path(args.out_dir) / metadata_filename) << "\n";
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }

    return 0;
}