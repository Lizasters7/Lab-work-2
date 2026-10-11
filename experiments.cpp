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
Experiments::Experiments(const SequenceStore& files, Timer timer)
    : timer(timer) {
    for (std::size_t size : SequenceStore::kSizes) {
        sequences.push_back(files.load(size));
    }
}

void Experiments::checkCorrectness() const {
    constexpr std::size_t kThreadCounts[] = {1, 2, 3, 7, 16, 100};

    auto check = [this, &kThreadCounts](const auto& predicate) {
        for (const auto& data : sequences) {
            const bool expected = std::any_of(data.begin(), data.end(), predicate);
            for (std::size_t k : kThreadCounts) {
                if (myAnyOf(data, k, predicate) != expected) {
                    throw std::runtime_error(
                        std::format("myAnyOf mismatch: size = {}, K = {}", data.size(), k));
                }
            }
        }
    };

    check(neverMatches);
    check(equalsMiddle);
    std::cout << "Correctness check passed: myAnyOf matches std::any_of\n\n";
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

void Experiments::runThreadCountExperiment() const {
    const std::size_t processorThreads = processorThreadCount();
    const std::vector<std::size_t> threadCounts = threadCountsToTest(processorThreads);

    TimeTable times(sequences.size());
    for (std::size_t s = 0; s < sequences.size(); ++s) {
        const auto& data = sequences[s];
        for (std::size_t k : threadCounts) {
            const TimeResult timeResult = timer.measure([&] { return myAnyOf(data, k, neverMatches); });
            times[s].push_back(timeResult.milliseconds);
        }
    }

    std::cout << std::format("myAnyOf with K threads, predicate x > {} (never true), median time, ms\n",
                             SequenceGenerator::kMaxValue);
    printThreadTable(threadCounts, times);
    printBestThreadCounts(threadCounts, times, processorThreads);
}

std::size_t Experiments::processorThreadCount() {
    return std::max(1u, std::thread::hardware_concurrency());
}

std::vector<std::size_t> Experiments::threadCountsToTest(std::size_t processorThreads) {
    std::vector<std::size_t> threadCounts;
    for (std::size_t i = 1; i <= 2 * processorThreads; ++i) {
        threadCounts.push_back(i);
    }
    for (std::size_t factor : {4, 8, 16, 32}) {
        threadCounts.push_back(factor * processorThreads);
    }
    return threadCounts;
}

void Experiments::printThreadTable(const std::vector<std::size_t>& threadCounts,
                                   const TimeTable& times) const {
    std::cout << std::format("{:>10}", "K");
    for (const auto& data : sequences) {
        std::cout << std::format(" {:>10}", data.size());
    }
    std::cout << '\n';
    for (std::size_t i = 0; i < threadCounts.size(); ++i) {
        std::cout << std::format("{:>10}", threadCounts[i]);
        for (const auto& sizeTimes : times) {
            std::cout << std::format(" {:>10.4f}", sizeTimes[i]);
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

void Experiments::printBestThreadCounts(const std::vector<std::size_t>& threadCounts,
                                        const TimeTable& times, std::size_t processorThreads) const {
    std::cout << std::format("Processor threads: {}\n", processorThreads);
    for (std::size_t i = 0; i < sequences.size(); ++i) {
        const auto fastest = std::min_element(times[i].begin(), times[i].end());
        const std::size_t bestK = threadCounts[std::distance(times[i].begin(), fastest)];
        const double ratio = static_cast<double>(bestK) / processorThreads;

        std::cout << std::format("size {:>10}: best K = {:>3} ({:.4f} ms), K / processor threads = {:.2f}\n",
                                 sequences[i].size(), bestK, *fastest, ratio);
    }
    std::cout << '\n';
}