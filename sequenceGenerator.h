#pragma once
#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

class SequenceGenerator {
public:
    static constexpr int kMinValue = 0;
    static constexpr int kMaxValue = 999;

    explicit SequenceGenerator(std::uint32_t seed);
    std::vector<int> generate(std::size_t size);

private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> distribution;
};