#ifndef METRICS_HPP
#define METRICS_HPP

#include <cstdint>

double compute_throughput(std::uint64_t total_elements, double partition_time_seconds);

#endif