#include "experiments.h"
#include "myAnyOf.h"
#include "sequenceGenerator.h"
#include <algorithm>
#include <execution>
#include <format>
#include <iostream>
#include <stdexcept>

constexpr auto neverMatches = [](int x) { return x > SequenceGenerator::kMaxValue; };
constexpr auto equalsMiddle = [](int x) { return x == SequenceGenerator::kMaxValue / 2; };
Experiments::Experiments(const SequenceStore& store, Timer timer)
    : timer(timer) {
    for (std::size_t size : SequenceStore::kSizes) {
        sequences.push_back(store.load(size));
    }
}

void Experiments::checkCorrectness() const {
    constexpr std::size_t kThreadCounts[] = {1, 2, 3, 7, 16, 100};

    auto check = [this, &kThreadCounts](const auto& predicate) {
        for (const auto& data : sequences) {
            const bool expected = std::any_of(data.begin(), data.end(), predicate);
            for (std::size_t k : kThreadCounts) {
                if (parallelAnyOf(data, k, predicate) != expected) {
                    throw std::runtime_error(
                        std::format("parallelAnyOf mismatch: size = {}, K = {}", data.size(), k));
                }
            }
        }
    };

    check(neverMatches);
    check(equalsMiddle);
    std::cout << "Correctness check passed: parallelAnyOf matches std::any_of\n\n";
}

void Experiments::runLibraryExperiment() const {
    constexpr const char* kColumns[] = {"no policy", "seq", "par", "unseq", "par_unseq"};

    std::cout << std::format("std::any_of, predicate x > {} (never true), median time, ms\n",
                             SequenceGenerator::kMaxValue);
    std::cout << std::format("{:>10}", "size");
    for (const char* column : kColumns) {
        std::cout << std::format(" {:>10}", column);
    }
    std::cout << '\n';

    for (const auto& data : sequences) {
        const auto begin = data.begin();
        const auto end = data.end();
        const TimeResult results[] = {
            timer.measure([&] { return std::any_of(begin, end, neverMatches); }),
            timer.measure([&] { return std::any_of(std::execution::seq, begin, end, neverMatches); }),
            timer.measure([&] { return std::any_of(std::execution::par, begin, end, neverMatches); }),
            timer.measure([&] { return std::any_of(std::execution::unseq, begin, end, neverMatches); }),
            timer.measure([&] { return std::any_of(std::execution::par_unseq, begin, end, neverMatches); }),
        };
        std::cout << std::format("{:>10}", data.size());
        for (const auto& result : results) {
            std::cout << std::format(" {:>10.4f}", result.milliseconds);
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}