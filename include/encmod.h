#ifndef AES_GCM_H
#define AES_GCM_H

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

class AES_GCM {
private:
    static constexpr size_t AES_KEY_SIZE = 32;  
    static constexpr size_t IV_SIZE = 12;      
    static constexpr size_t TAG_SIZE = 16;     

    unsigned char aesKey[AES_KEY_SIZE];  
    std::vector<fs::path> generatedFiles;  

    void handleErrors();  

public:
    explicit AES_GCM(const std::vector<unsigned char>& key);
    ~AES_GCM();

    fs::path encrypt(const fs::path& inputFilePath);
    fs::path decrypt(const fs::path& inputFilePath);

    void cleanupFiles();  
};

#endif // AES_GCM_H
