#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <functional>
#include <immintrin.h>
#include <stdexcept>

#include "lib/dataset.hpp"
#include "lib/timing.hpp"
#include "lib/checksum.hpp"
#include "lib/config.hpp"
#include "lib/results.hpp"


struct Args {
    uint64_t N = 10'000'000;
    uint32_t P = 128;
    std::string hash_name = "mask";
    std::string exec_type = "plain_novec";
    std::string output_csv = RESULTS_CSV_FILE; // default value defined in config.hpp
};

static Args parse_args(int argc, char** argv) {
    Args args;
    args.exec_type = std::string(argv[0]).substr(2);
    if (argc > 1) args.N = std::stoull(argv[1]);
    if (argc > 2) args.P = std::stoull(argv[2]);
    if (argc > 3) args.hash_name = std::string(argv[3]);
    if (argc > 4) args.output_csv = std::string(argv[4]);
    return args;
}

void check_args(const Args& args) {
    if (args.N == 0) {
        throw std::invalid_argument("N must be > 0");
    }
    if (args.P == 0) {
        throw std::invalid_argument("P must be > 0");
    }
    if (args.hash_name != "mask" && args.hash_name != "mul" && args.hash_name != "fmix64") {
        throw std::invalid_argument("Unsupported hash function: " + args.hash_name);
    }
    if (args.exec_type != "plain_novec" && args.exec_type != "plain_vec" && args.exec_type != "avx2") {
        throw std::invalid_argument("Unsupported execution type: " + args.exec_type);
    }
    if (args.output_csv.empty()) {
        throw std::invalid_argument("Output CSV path cannot be empty");
    }
}

std::vector<uint32_t> compute_partitions_avx2(const std::vector<uint64_t>& keys, std::vector<uint32_t>& part_id, uint32_t P, const std::string& hash_name) {
    // P validation check
    if (P == 0 || (P & (P - 1)) != 0) {
        throw std::invalid_argument("P must be a power of two");
    }
    const uint64_t P_mask = static_cast<uint64_t>(P - 1);

    // Build the mask vector to be applied to 4 progressive uint64_t keys
    const __m256i mask_vec = _mm256_set1_epi64x(static_cast<long long>(P_mask));

    // For each step are processed 4 uint64_t keys (= 1 lane)
    std::size_t i = 0;
    const std::size_t simd_width = 4; // because AVX2 uses registers of 256 bits (256 / 64 = 4)
    for (; i + simd_width <= keys.size(); i += simd_width) {
        // Build the vector of 4 progressive uint64_t keys (from i to i+3)
        __m256i key_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&keys[i]));

        // Bitwise AND with mask
        __m256i part_vec = _mm256_and_si256(key_vec, mask_vec);

        // Store temporary 64-bit results
        alignas(32) uint64_t tmp[4];
        _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), part_vec);

        // Cast the current lane to uint32_t
        part_id[i+0] = static_cast<uint32_t>(tmp[0]);
        part_id[i+1] = static_cast<uint32_t>(tmp[1]);
        part_id[i+2] = static_cast<uint32_t>(tmp[2]);
        part_id[i+3] = static_cast<uint32_t>(tmp[3]);
    }
    
    // Cycle done for the last possible positions remained (< 4)
    for (; i < keys.size(); ++i) {
        part_id[i] = static_cast<uint32_t>(keys[i] & P_mask);
    }

    return part_id;
}



// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    const Args args = parse_args(argc, argv);
    check_args(args);

    std::vector<std::uint32_t> R_partitioned(args.N), S_partitioned(args.N);
    double t0_global, t1_global, global_time, t0, t1, t, partition_time;
    const int n_digits = 5;

    // Loading datasets
    t0 = t0_global = get_time();
    Dataset R = load_dataset("dataset/R.bin");
    Dataset S = load_dataset("dataset/S.bin");
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    if (VERBOSE) {
        std::cout << "LOADING_TIME[s]=" << t << "\n";
    }

    // Get the subset of the dataset if N is smaller than the dataset size
    if (args.N > R.size || args.N > S.size) {
        throw std::runtime_error("N cannot be larger than the dataset size");
    }
    R.keys.resize(args.N);
    S.keys.resize(args.N);
    R.size = S.size = args.N;
    if (VERBOSE) {
        std::cout << "[resize] Are used the first " << args.N << " keys of each dataset\n";
    }

    // Choose the right partition function
    using PartitionFn = std::function<std::vector<uint32_t>(const std::vector<uint64_t>&, std::vector<uint32_t>&, uint32_t, const std::string&)>;
    PartitionFn partition_fn = compute_partitions_avx2;

    // Compute the partitions for each dataset
    t0 = get_time();
    partition_fn(R.keys, R_partitioned, args.P, args.hash_name);
    t1 = get_time();
    partition_time = get_diff(t0, t1, n_digits);
    t0 = get_time();
    partition_fn(S.keys, S_partitioned, args.P, args.hash_name);
    t1 = get_time();
    partition_time += get_diff(t0, t1, n_digits);
    std::cout << "PARTITION_TIME[s]=" << partition_time << " ";
    
    // Compute the throughput of computing partitions
    const double throughput = compute_throughput(
        static_cast<std::uint64_t>(R.size) + static_cast<std::uint64_t>(S.size),
        partition_time
    );
    std::cout << "THROUGHPUT[elems/s]=" << throughput << "\n";

    // Generate the checksum of the datasets
    t0 = get_time();
    uint64_t checksum_R = compute_checksum(R_partitioned);
    uint64_t checksum_S = compute_checksum(S_partitioned);
    std::string checksum = std::to_string(checksum_R) + std::to_string(checksum_S);
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    if (VERBOSE) {
        std::cout << "CHECKSUM_TIME[s]=" << t << "\n";
    }

    // Save the partitioned datasets
    t0 = get_time();
    const std::string R_filename = "dataset/R_partitioned.bin";
    const std::string S_filename = "dataset/S_partitioned.bin";
    write_binary_dataset(std::filesystem::path(R_filename), R_partitioned);
    write_binary_dataset(std::filesystem::path(S_filename), S_partitioned);
    t1 = t1_global = get_time();
    t = get_diff(t0, t1, n_digits);
    global_time = get_diff(t0_global, t1_global, n_digits);
    if (VERBOSE) {
        std::cout << "SAVE_TIME[s]=" << t << "\n";
    }

    // Final outputs
    if (VERBOSE) {
        std::cout << "CHECKSUM=" << checksum << "\n";
        std::cout << "THROUGHPUT[elems/s]=" << throughput << "\n";
        std::cout << "GLOBAL_TIME[s]=" << global_time << "\n";
    }
    append_to_csv(
        args.output_csv,
        args.N,
        args.P,
        args.hash_name,
        args.exec_type,
        checksum,
        throughput,
        partition_time,
        global_time
    );
    return 0;
}
