#ifndef METRICS_HPP
#define METRICS_HPP

#include <cstdint>
#include <filesystem>
#include <fstream>

double compute_throughput(std::uint64_t total_elements, double partition_time_seconds);


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