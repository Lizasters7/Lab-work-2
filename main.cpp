// Compiler: MSVC (Microsoft Visual Studio 2022), Standard: C++23
#include <cstddef>
#include <exception>
#include <iostream>
#include "experiments.h"
#include "timer.h"
#include "sequenceStore.h"

int main() {
    constexpr std::size_t kRepetitions = 7;
    try {
        const SequenceStore store("data");
        store.createMissingFiles();
        const Experiments experiments(store, Timer(kRepetitions));
        experiments.checkCorrectness();
        experiments.runLibraryExperiment();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}