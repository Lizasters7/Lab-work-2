#include "sequenceStore.h"
#include <fstream>
#include <string>
#include "SequenceGenerator.h"

SequenceStore::SequenceStore(std::filesystem::path directory)
    : directory(std::move(directory)) {}

void SequenceStore::createMissingFiles() const {
    std::filesystem::create_directories(directory);

    for (std::size_t i = 0; i < kSizes.size(); ++i) {
        const auto path = pathFor(kSizes[i]);
        try {
            openForReading(path);  
        } catch (const std::runtime_error&) {
            SequenceGenerator generator(kBaseSeed + static_cast<std::uint32_t>(i));
            write(path, generator.generate(kSizes[i]));
        }
    }
}

std::vector<int> SequenceStore::load(std::size_t size) const {
    auto in = openForReading(pathFor(size));
    std::size_t count = 0;
    in >> count;

    std::vector<int> sequence(count);
    for (int& value : sequence) {
        in >> value;
    }
    return sequence;
}

std::filesystem::path SequenceStore::pathFor(std::size_t size) const {
    return directory / ("data_" + std::to_string(size) + ".txt");
}

std::ifstream SequenceStore::openForReading(const std::filesystem::path& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Cannot open data file for reading: " + path.string());
    }
    return in;
}

void SequenceStore::write(const std::filesystem::path& path, const std::vector<int>& sequence) {
    constexpr std::size_t kNumbersPerLine = 20;

    std::ofstream out(path);
    if (!out) {
        throw std::runtime_error("Cannot create data file: " + path.string());
    }
    out << sequence.size() << '\n';
    for (std::size_t i = 0; i < sequence.size(); ++i) {
        const bool endOfLine = (i + 1) % kNumbersPerLine == 0 || i + 1 == sequence.size();
        for (int value : sequence) {
            out << value << '\n';
        }
    }
}