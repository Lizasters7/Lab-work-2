#pragma once
#include <algorithm>
#include <cstddef>
#include <thread>
#include <vector>

template <typename Predicate>
bool myAnyOf(const std::vector<int>& data, std::size_t threadCount, Predicate predicate) {
    std::vector<char> found(threadCount, 0);  
    std::vector<std::thread> threads;
    threads.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i) {
        const auto first = data.begin() + static_cast<std::ptrdiff_t>(data.size() * i / threadCount);
        const auto last = data.begin() + static_cast<std::ptrdiff_t>(data.size() * (i + 1) / threadCount);
        threads.emplace_back([&found, i, first, last, predicate] {
            found[i] = std::any_of(first, last, predicate);
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    auto isFound = [](char f) { return f != 0; };
    return std::any_of(found.begin(), found.end(), isFound);
}