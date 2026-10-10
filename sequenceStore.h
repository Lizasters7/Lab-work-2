#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

class SequenceStore {
public:
    static constexpr std::array<std::size_t, 4> kSizes = {1000, 100000, 1000000, 10000000};
    explicit SequenceStore(std::filesystem::path directory);
    void createMissingFiles() const;
    std::vector<int> load(std::size_t size) const;

private:
    static constexpr std::uint32_t kBaseSeed = 2026;
    std::filesystem::path pathFor(std::size_t size) const;
    static std::ifstream openForReading(const std::filesystem::path& path);
    static void write(const std::filesystem::path& path, const std::vector<int>& sequence);
    std::filesystem::path directory;
};