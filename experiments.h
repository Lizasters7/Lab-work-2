#pragma once
#include <vector>
#include "timer.h"
#include "sequenceStore.h"

class Experiments {
public:
    Experiments(const SequenceStore& files, Timer timer);
    void checkCorrectness() const;
    void runLibraryExperiment() const;
    void runThreadCountExperiment() const;

private:
    using TimeTable = std::vector<std::vector<double>>;
    static std::size_t processorThreadCount();
    static std::vector<std::size_t> threadCountsToTest(std::size_t processorThreads);

    void printThreadTable(const std::vector<std::size_t>& threadCounts, const TimeTable& times) const;
    void printBestThreadCounts(const std::vector<std::size_t>& threadCounts, const TimeTable& times,
                               std::size_t processorThreads) const;

    std::vector<std::vector<int>> sequences;
    Timer timer;
};