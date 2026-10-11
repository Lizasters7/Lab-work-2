#pragma once
#include <vector>
#include "timer.h"
#include "sequenceStore.h"

class Experiments {
public:
    Experiments(const SequenceStore& store, Timer timer);
    void checkCorrectness() const;
    void runLibraryExperiment() const;

private:
    std::vector<std::vector<int>> sequences;
    Timer timer;
};