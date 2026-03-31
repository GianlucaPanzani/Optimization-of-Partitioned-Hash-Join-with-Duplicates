#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "lib/dataset.hpp"
#include "lib/timing.hpp"
#include "lib/checksum.hpp"
#include "lib/config.hpp"
#include "lib/results.hpp"
#include "lib/partition.hpp"
#ifdef ENABLE_AVX2
#include "lib/partition_avx2.hpp"
#endif


struct Args {
    uint64_t N = 10'000'000;
    uint32_t P = 128;
    std::string hash_name = "mask";
    std::string exec_type = "plain_novec";
};

static Args parse_args(int argc, char** argv) {
    Args args;
    if (argc > 1) args.N = std::stoull(argv[1]);
    if (argc > 2) args.P = std::stoull(argv[2]);
    if (argc > 3) args.hash_name = std::string(argv[3]);
    if (argc > 4) args.exec_type = std::string(argv[4]);
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
}



// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    const Args args = parse_args(argc, argv);
    check_args(args);

    std::vector<std::uint32_t> R_partitioned, S_partitioned;
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

    // Compute the partitions for each dataset
    t0 = get_time();
    if (args.exec_type == "avx2") {
        #ifdef ENABLE_AVX2
            R_partitioned = compute_partitions_avx2(R.keys, args.P, args.hash_name);
            S_partitioned = compute_partitions_avx2(S.keys, args.P, args.hash_name);
        #else
            throw std::invalid_argument("This binary was compiled without AVX2 support");
        #endif
    } else {
        R_partitioned = compute_partitions(R.keys, args.P, args.hash_name);
        S_partitioned = compute_partitions(S.keys, args.P, args.hash_name);
    }
    t1 = get_time();
    partition_time = get_diff(t0, t1, n_digits);
    std::cout << "PARTITION_TIME[s]=" << partition_time << "\n";
    
    // Compute the throughput of computing partitions
    const double throughput = compute_throughput(
        static_cast<std::uint64_t>(R.size) + static_cast<std::uint64_t>(S.size),
        partition_time
    );

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

    // Outputs
    if (VERBOSE) {
        std::cout << "CHECKSUM=" << checksum << "\n";
        std::cout << "THROUGHPUT[elems/s]=" << throughput << "\n";
        std::cout << "GLOBAL_TIME[s]=" << global_time << "\n";
    }
    append_to_csv(
        RESULTS_CSV_FILE,
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
