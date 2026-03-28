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
#include "lib/metrics.hpp"
#include "lib/partition.hpp"
#include "lib/config.hpp"



struct Args {
    uint32_t P = 128;
};


static Args parse_args(int argc, char** argv) {
    Args cfg;
    if (argc > 1) cfg.P = std::stoull(argv[1]);
    return cfg;
}


static std::string get_executable_name(char** argv) {
    if (argv == nullptr || argv[0] == nullptr) {
        return "plain";
    }

    const std::filesystem::path exe_path(argv[0]);
    const std::string exe_name = exe_path.filename().string();
    return exe_name.empty() ? "plain" : exe_name;
}


template <typename T>
static void append_result(const char* output_file, const std::string& exe_name, const T result) {
    const std::filesystem::path output_path(output_file);
    const std::filesystem::path parent = output_path.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    std::ofstream out(output_path, std::ios::app);
    if (!out) {
        throw std::runtime_error("Cannot open output file: " + output_path.string());
    }

    out << exe_name << ": " << result << '\n';
    if (!out) {
        throw std::runtime_error("Error while writing file: " + output_path.string());
    }
}


// =========================================================
// ========================= MAIN ==========================
// =========================================================
int main(int argc, char** argv) {
    const Args cfg = parse_args(argc, argv);
    const std::uint32_t P = cfg.P;
    const std::string exe_name = get_executable_name(argv);

    // Timing variables
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

    // Compute the partitions for each dataset
    t0 = get_time();
    const std::vector<std::uint32_t> R_partitioned = compute_partitions(R.keys, P);
    const std::vector<std::uint32_t> S_partitioned = compute_partitions(S.keys, P);
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
        std::cout << "[checksum] R:\n\t" << checksum_R << "\n";
        std::cout << "[checksum] S:\n\t" << checksum_S << "\n";
        std::cout << "[metric] throughput:\n\t" << throughput << " elems/s\n";
    }
    std::cout << "[time] global:\n\t" << global_time << " s\n";
    return 0;
}
