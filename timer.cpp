#include "timer.h"
#include <algorithm>
#include <chrono>
#include <vector>

Timer::Timer(std::size_t repetitions) : repetitions(repetitions) {}

TimeResult Timer::measure(const std::function<bool()>& task) const {
    std::vector<double> times;
    times.reserve(repetitions);
    bool result = false;
    for (std::size_t i = 0; i < repetitions; ++i) {
        const auto start = std::chrono::steady_clock::now();
        result = task();
        const auto finish = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(finish - start).count());
    }
    std::sort(times.begin(), times.end());
    const double median = times.at(times.size() / 2);
    return TimeResult{median, result};
}