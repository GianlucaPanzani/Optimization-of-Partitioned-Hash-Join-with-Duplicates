#include <stdexcept>

#include "metrics.hpp"


double compute_throughput(std::uint64_t total_elements, double partition_time_seconds) {
    if (partition_time_seconds <= 0.0) {
        throw std::invalid_argument("partition_time_seconds must be > 0");
    }
    return static_cast<double>(total_elements) / partition_time_seconds;
}