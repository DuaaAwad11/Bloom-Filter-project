#include "HashFunctions.hpp"
#include <openssl/evp.h>
#include <cstring>
#include <memory>
#include <stdexcept>

uint64_t HashFunctions::sha256Hash(const std::string& input) {
    if (input.find('\0') != std::string::npos) {
        throw std::invalid_argument("Input contains raw null-byte injections");
    }

    std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    if (!ctx) {
        throw std::runtime_error("EVP_MD_CTX_new failed for SHA256");
    }

    const EVP_MD* md = EVP_sha256();
    if (!md) {
        throw std::runtime_error("EVP_sha256 failed");
    }

    if (EVP_DigestInit_ex(ctx.get(), md, nullptr) != 1) {
        throw std::runtime_error("EVP_DigestInit_ex failed for SHA256");
    }

    if (EVP_DigestUpdate(ctx.get(), input.c_str(), input.size()) != 1) {
        throw std::runtime_error("EVP_DigestUpdate failed for SHA256");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(ctx.get(), hash, &length) != 1) {
        throw std::runtime_error("EVP_DigestFinal_ex failed for SHA256");
    }

    if (length < sizeof(uint64_t)) {
        throw std::runtime_error("SHA256 hash length is smaller than uint64_t size");
    }

    uint64_t result = 0;
    std::memcpy(&result, hash, sizeof(uint64_t));
    return result;
}

uint64_t HashFunctions::md5Hash(const std::string& input) {
    if (input.find('\0') != std::string::npos) {
        throw std::invalid_argument("Input contains raw null-byte injections");
    }

    std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    if (!ctx) {
        throw std::runtime_error("EVP_MD_CTX_new failed for MD5");
    }

    const EVP_MD* md = EVP_md5();
    if (!md) {
        throw std::runtime_error("EVP_md5 failed");
    }

    if (EVP_DigestInit_ex(ctx.get(), md, nullptr) != 1) {
        throw std::runtime_error("EVP_DigestInit_ex failed for MD5");
    }

    if (EVP_DigestUpdate(ctx.get(), input.c_str(), input.size()) != 1) {
        throw std::runtime_error("EVP_DigestUpdate failed for MD5");
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(ctx.get(), hash, &length) != 1) {
        throw std::runtime_error("EVP_DigestFinal_ex failed for MD5");
    }

    if (length < sizeof(uint64_t)) {
        throw std::runtime_error("MD5 hash length is smaller than uint64_t size");
    }

    uint64_t result = 0;
    std::memcpy(&result, hash, sizeof(uint64_t));
    return result;
}
