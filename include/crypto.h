#ifndef AES_GCM_H
#define AES_GCM_H

#include <vector>
#include <array>
#include <fstream>
#include <iostream>
#include <optional>
#include <filesystem>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

namespace fs = std::filesystem;

class Crypto {
private:
    static constexpr size_t AES_KEY_SIZE = 32;  
    static constexpr size_t IV_SIZE = 12;      
    static constexpr size_t TAG_SIZE = 16;     

    unsigned char aesKey[AES_KEY_SIZE];  
    std::vector<fs::path> generatedFiles;  

    void handleErrors();  

public:
    explicit Crypto(const std::vector<unsigned char>& key);
    ~Crypto();    

    fs::path encrypt(const fs::path& inputFilePath);
    fs::path decrypt(const fs::path& inputFilePath);

    void cleanupFiles();  
};

#endif 
