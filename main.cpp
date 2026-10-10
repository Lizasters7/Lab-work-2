#include <algorithm>
#include <execution>
#include <iostream>
#include <thread>
#include "SequenceGenerator.h"
#include "SequenceStore.h"

int main() {
    try {
        const SequenceStore store("data");
        store.createMissingFiles();
        const auto sequence = store.load(SequenceStore::kSizes.back());
        std::cout << "Loaded " << sequence.size() << " numbers\n";
        auto never = [](int x) { return x > SequenceGenerator::kMaxValue; };
        std::cout
              << "no policy: " << std::any_of(sequence.begin(), sequence.end(), never) << '\n'
              << "seq:       " << std::any_of(std::execution::seq, sequence.begin(), sequence.end(), never) << '\n'
              << "par:       " << std::any_of(std::execution::par, sequence.begin(), sequence.end(), never) << '\n'
              << "unseq:     " << std::any_of(std::execution::unseq, sequence.begin(), sequence.end(), never) << '\n'
              << "par_unseq: " << std::any_of(std::execution::par_unseq, sequence.begin(), sequence.end(), never) << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

}