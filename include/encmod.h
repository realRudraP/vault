#ifndef AES_GCM_ENCMOD_H
#define AES_GCM_ENCMOD_H

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

namespace fs = std::filesystem;

constexpr int AES_KEY_SIZE = 32;      // AES-256
constexpr int IV_SIZE = 12;           // Recommended for GCM
constexpr int TAG_SIZE = 16;
constexpr size_t CHUNK_SIZE = 4 * 1024 * 1024;  // 4MB

class AES_GCM {
public:
    AES_GCM(const std::vector<unsigned char>& key);
    ~AES_GCM();

    fs::path encrypt(const fs::path& inputFilePath);
    fs::path decrypt(const fs::path& inputFilePath);

private:
    unsigned char aesKey[AES_KEY_SIZE];
    std::vector<fs::path> generatedFiles;

    void handleErrors();
    void cleanupFiles();
};

#endif
