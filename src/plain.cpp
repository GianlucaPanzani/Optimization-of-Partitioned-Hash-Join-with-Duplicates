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


struct Args {
    uint64_t N = 10'000'000;
    uint32_t P = 128;
    std::string hash_name = "";
};

static Args parse_args(int argc, char** argv) {
    Args args;
    if (argc > 1) args.N = std::stoull(argv[1]);
    if (argc > 2) args.P = std::stoull(argv[2]);
    if (argc > 3) args.hash_name = std::string(argv[3]);
    return args;
}



// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    const Args args = parse_args(argc, argv);
    const std::string exe_name = argv[0];

    double t0_global, t1_global, global_time, t0, t1, t;
    const int n_digits = 5;

    // Loading datasets
    t0 = t0_global = get_time();
    Dataset R = load_dataset("dataset/R.bin");
    Dataset S = load_dataset("dataset/S.bin");
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    if (VERBOSE) {
        std::cout << "[time] loading datasets:\n\t" << t << " s\n";
    }

    // Get the subset of the dataset if N is smaller than the dataset size
    if (args.N < R.size || args.N < S.size) {
        R.keys.resize(args.N);
        S.keys.resize(args.N);
        R.size = S.size = args.N;
        if (VERBOSE) {
            std::cout << "[resize] Are used only the first " << args.N << " keys of each dataset\n";
        }
    }

    // Compute the partitions for each dataset
    t0 = get_time();
    const std::vector<std::uint32_t> R_partitioned = compute_partitions(R.keys, args.P, args.hash_name);
    const std::vector<std::uint32_t> S_partitioned = compute_partitions(S.keys, args.P, args.hash_name);
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    std::cout << "[time] computing partitions:\n\t" << t << " s\n";
    
    // Compute the throughput of computing partitions
    double throughput = compute_throughput(static_cast<std::uint64_t>(R.size) + static_cast<std::uint64_t>(S.size), t);
    append_result(THROUGHPUT_FILE, exe_name, throughput);

    // Generate the checksum of the datasets
    t0 = get_time();
    uint64_t checksum_R = compute_checksum(R_partitioned);
    uint64_t checksum_S = compute_checksum(S_partitioned);
    std::string checksum = std::to_string(checksum_R) + "_" + std::to_string(checksum_S);
    append_result(CHECKSUM_FILE, exe_name, checksum);
    t1 = t1_global = get_time();
    t = get_diff(t0, t1, n_digits);
    if (VERBOSE) {
        std::cout << "[time] computing checksums:\n\t" << t << " s\n";
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

    // Outputs
    if (VERBOSE) {
        std::cout << "[checksum] total checksum:\n\t" << checksum << "\n";
        std::cout << "[metric] throughput:\n\t" << throughput << " elems/s\n";
        std::cout << "[time] global:\n\t" << global_time << " s\n";
    }
    update_results_json(
        "results/results.json",
        exe_name,
        args.N,
        args.P,
        throughput,
        global_time,
        checksum,
        args.hash_name
    );
    return 0;
}
