#ifndef RESULTS_HPP
#define RESULTS_HPP

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

double compute_throughput(std::uint64_t total_elements, double partition_time_seconds);

void update_results_json(
    const std::string& json_path,
    const std::string& executable_name,
    std::uint64_t N,
    std::uint32_t P,
    double throughput,
    double t,
    const std::string& checksum,
    const std::string& hash_name
);


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

#endif
