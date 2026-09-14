#include "BloomHasher.hpp"
#include "HashFunctions.hpp"
#include <cmath>
#include <numeric>

std::vector<uint64_t> BloomHasher::generatePositions(const std::string& bigram, uint64_t bloomSize, int k) {
    std::vector<uint64_t> pos;
    pos.reserve(k);

    uint64_t f = HashFunctions::sha256Hash(bigram);
    uint64_t g = HashFunctions::md5Hash(bigram);

    for (int i = 0; i < k; i++) {
        uint64_t h_i = (f + i * g) % bloomSize;
        pos.push_back(h_i);
    }
    return pos;
}

std::vector<bool> BloomHasher::getPasswordFilter(const std::string& password, uint64_t bloomSize, int k) {
    std::vector<bool> filter(bloomSize, false);
    if (password.empty()) return filter;
    
    std::string padded = " " + password + " ";

    for (size_t i = 0; i < padded.size() - 1; i++) {
        std::string bg = padded.substr(i, 2);
        std::vector<uint64_t> pos = generatePositions(bg, bloomSize, k);
        for (uint64_t p : pos) {
            filter[p] = true;
        }
    }
    return filter;
}

double BloomHasher::computeJaccard(const std::vector<bool>& filterA, const std::vector<bool>& filterB) {
    if (filterA.size() != filterB.size() || filterA.empty()) return 0.0;
    
    size_t intersect = 0;
    size_t union_cnt = 0;
    
    for (size_t i = 0; i < filterA.size(); i++) {
        if (filterA[i] && filterB[i]) intersect++;
        if (filterA[i] || filterB[i]) union_cnt++;
    }
    
    return union_cnt == 0 ? 0.0 : (double)intersect / union_cnt;
}

double BloomHasher::computeDice(const std::vector<bool>& filterA, const std::vector<bool>& filterB) {
    if (filterA.size() != filterB.size() || filterA.empty()) return 0.0;
    
    size_t intersect = 0;
    size_t countA = 0;
    size_t countB = 0;
    
    for (size_t i = 0; i < filterA.size(); i++) {
        if (filterA[i] && filterB[i]) intersect++;
        if (filterA[i]) countA++;
        if (filterB[i]) countB++;
    }
    
    return (countA + countB) == 0 ? 0.0 : (2.0 * intersect) / (countA + countB);
}

double BloomHasher::computeCosine(const std::vector<bool>& filterA, const std::vector<bool>& filterB) {
    if (filterA.size() != filterB.size() || filterA.empty()) return 0.0;
    
    size_t intersect = 0;
    size_t countA = 0;
    size_t countB = 0;
    
    for (size_t i = 0; i < filterA.size(); i++) {
        if (filterA[i] && filterB[i]) intersect++;
        if (filterA[i]) countA++;
        if (filterB[i]) countB++;
    }
    
    if (countA == 0 || countB == 0) return 0.0;
    return (double)intersect / std::sqrt((double)countA * countB);
}
