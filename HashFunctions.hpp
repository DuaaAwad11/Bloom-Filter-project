#pragma once
#include <string>
#include <cstdint>

class HashFunctions {
public:
    static uint64_t sha256Hash(const std::string& input);

    static uint64_t md5Hash(const std::string& input);
};
