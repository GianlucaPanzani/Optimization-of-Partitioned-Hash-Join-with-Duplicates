#include "results.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <nlohmann/json.hpp>


double compute_throughput(std::uint64_t total_elements, double partition_time_seconds) {
    if (partition_time_seconds <= 0.0) {
        throw std::invalid_argument("partition_time_seconds must be > 0");
    }
    return static_cast<double>(total_elements) / partition_time_seconds;
}


void update_results_json(
    const std::string& json_path,
    const std::string& executable_name,
    std::uint64_t N,
    std::uint32_t P,
    double throughput,
    double t,
    const std::string& checksum,
    const std::string& hash_name
) {
    using json = nlohmann::json;
    json j;

    if (std::filesystem::exists(json_path)) {
        std::ifstream in(json_path);
        if (!in) {
            throw std::runtime_error("Cannot open JSON file for reading: " + json_path);
        }
        in >> j;
    } else {
        j = json::object();
    }

    const std::string top_key = "N=" + std::to_string(N) + " P=" + std::to_string(P);

    j[top_key][executable_name]["throughput"] = throughput;
    j[top_key][executable_name]["time"] = t;
    j[top_key][executable_name]["checksum"] = checksum;
    j[top_key][executable_name]["hash"] = hash_name;

    std::ofstream out(json_path);
    if (!out) {
        throw std::runtime_error("Cannot open JSON file for writing: " + json_path);
    }

    out << j.dump(4) << "\n";
}