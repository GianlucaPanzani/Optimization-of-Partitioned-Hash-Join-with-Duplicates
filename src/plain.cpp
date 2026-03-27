#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "lib/dataset.hpp"
#include "lib/timing.hpp"
#include "lib/checksum.hpp"
#include "lib/metrics.hpp"
#include "lib/partition.hpp"



struct Args {
    uint32_t P = 128;
};


static Args parse_args(int argc, char** argv) {
    Args cfg;
    if (argc > 1) cfg.P = std::stoull(argv[1]);
    return cfg;
}


// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    const Args cfg = parse_args(argc, argv);
    const std::uint32_t P = cfg.P;

    // Timing variables
    double t0_global, t1_global, t0, t1, t;
    const int n_digits = 5;

    // Loading datasets
    t0 = t0_global = get_time();
    Dataset R = load_dataset("dataset/R.bin");
    Dataset S = load_dataset("dataset/S.bin");
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    std::cout << "[time] loading datasets:\t" << t << " s\n";

    // Compute the partitions for each dataset
    t0 = get_time();
    std::vector<std::uint32_t> R_partitions = compute_partitions(R.keys, P);
    std::vector<std::uint32_t> S_partitions = compute_partitions(S.keys, P);
    t1 = get_time();
    t = get_diff(t0, t1, n_digits);
    std::cout << "[time] computing partitions:\t" << t << " s\n";
    
    // Compute the throughput of computing partitions
    double throughput = compute_throughput(static_cast<std::uint64_t>(R.size) + static_cast<std::uint64_t>(S.size), t);

    // Generate the checksum of the datasets
    t0 = get_time();
    uint64_t checksum_R = compute_checksum(R_partitions);
    uint64_t checksum_S = compute_checksum(S_partitions);
    t1 = t1_global = get_time();
    t = get_diff(t0, t1, n_digits);
    std::cout << "[time] computing checksums:\t" << t << " s\n";

    // Outputs
    std::cout << "[checksum] R:\t" << checksum_R << "\n";
    std::cout << "[checksum] S:\t" << checksum_S << "\n";
    std::cout << "[metric] throughput:\t" << throughput << " elems/s\n";
    std::cout << "[time] global:\t" << get_diff(t0_global, t1_global, n_digits) << " s\n";
    return 0;
}
