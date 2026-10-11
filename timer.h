#pragma once
#include <cstddef>
#include <functional>

struct TimeResult {
    double milliseconds; 
    bool result;          
};

class Timer {
public:
    explicit Timer(std::size_t repetitions);

    TimeResult measure(const std::function<bool()>& task) const;

private:
    std::size_t repetitions;
};