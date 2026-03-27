#include <chrono>
#include <cmath>

#include "timing.hpp"


double get_time() {
    using clock = std::chrono::steady_clock;
    auto now = clock::now();
    auto time = std::chrono::duration<double>(now.time_since_epoch());
    return time.count();
}


double get_diff(double t0, double t1, int n_digits) {
    const double diff = t1 - t0;
    const double scale = std::pow(10.0, n_digits);
    return std::round(diff * scale) / scale;
}