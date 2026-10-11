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
        const SequenceStore files("data");
        files.createMissingFiles();
        const Experiments experiments(files, Timer(kRepetitions));
        experiments.checkCorrectness();
        experiments.runLibraryExperiment();
        experiments.runThreadCountExperiment();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}