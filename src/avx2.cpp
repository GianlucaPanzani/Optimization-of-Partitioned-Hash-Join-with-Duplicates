#include <cstdint>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "lib/config.hpp"
#include "lib/dataset.hpp"
#include "lib/timing.hpp"
#include "lib/checksum.hpp"
#include "lib/results.hpp"
#include "lib/partition_avx2.hpp"


struct Args {
    uint32_t P = 128;
    std::string hash_name = "";
};

static Args parse_args(int argc, char** argv) {
    Args args;
    if (argc > 1) args.P = static_cast<uint32_t>(std::stoul(argv[1]));
    if (argc > 2) args.hash_name = std::stoull(argv[2]);
    return args;
}


// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    const Args args = parse_args(argc, argv);
    const uint32_t P = args.P;
    const std::string exe_name = argv[0];

    double t0_global, t1_global, global_time, t0, t1, t;
    const int n_digits = 5;

    t0_global = get_time();

    // Loading datasets
    t0 = get_time();
    Dataset R = load_dataset("dataset/R.bin");
    Dataset S = load_dataset("dataset/S.bin");
    t1 = get_time();
    if (VERBOSE) {
        std::cout << "[time] loading datasets:\n\t" << get_diff(t0, t1, n_digits) << " s\n";
    }

    // Compute AVX2 partitions
    t0 = get_time();
    std::vector<uint32_t> R_partitioned = compute_partitions_avx2(R.keys, P);
    std::vector<uint32_t> S_partitioned = compute_partitions_avx2(S.keys, P);
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    if (VERBOSE) {
        std::cout << "[time] computing partitions:\n\t" << t << " s\n";
    }

    // Checksums
    const uint64_t checksum_R = compute_checksum(R_partitioned);
    const uint64_t checksum_S = compute_checksum(S_partitioned);
    const std::string checksum = std::to_string(checksum_R) + "_" + std::to_string(checksum_S);
    if (VERBOSE) {
        std::cout << "[checksum] total checksum:\n\t" << checksum << "\n";
    }

    // Throughput
    const double throughput = compute_throughput(R.size + S.size, t);
    if (VERBOSE) {
        std::cout << "[throughput] partition mapping:\t" << throughput << " elem/s\n";
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
    append_result(TIMING_FILE, exe_name, global_time);
    if (VERBOSE) {
        std::cout << "[time] saving parts datasets:\n\t" << t << " s\n";
    }
    
    // Global time output
    t1_global = get_time();
    if (VERBOSE) {
        std::cout << "[time] global:\n\t" << get_diff(t0_global, t1_global, n_digits) << " s\n";
    }
    update_results_json(
        "results/results.json",
        exe_name,
        R.size,
        P,
        throughput,
        global_time,
        checksum,
        args.hash_name
    );
    return 0;
}