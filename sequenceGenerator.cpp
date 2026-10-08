#include "SequenceGenerator.h"

#include <algorithm>

SequenceGenerator::SequenceGenerator(std::uint32_t seed)
    : rng(seed), distribution(kMinValue, kMaxValue) {}

std::vector<int> SequenceGenerator::generate(std::size_t size) {
    std::vector<int> sequence(size);
    for (int& value : sequence) 
    {
        value = distribution(rng);
    }
    return sequence;
}