#pragma once
#include <string>
#include <vector>
#include <cstdint>

class BloomHasher {
public:
    static std::vector<uint64_t> generatePositions(const std::string& bigram, uint64_t bloomSize = 1000, int k = 20);
    static std::vector<bool> getPasswordFilter(const std::string& password, uint64_t bloomSize = 1000, int k = 20);

    static double computeJaccard(const std::vector<bool>& filterA, const std::vector<bool>& filterB);
    static double computeDice(const std::vector<bool>& filterA, const std::vector<bool>& filterB);
    static double computeCosine(const std::vector<bool>& filterA, const std::vector<bool>& filterB);
};
