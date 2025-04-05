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
    public:
    static constexpr size_t AES_KEY_SIZE = 32;  
    static constexpr size_t IV_SIZE = 12;      
    static constexpr size_t TAG_SIZE = 16;
    static constexpr size_t SALT_SIZE = 16;
private:     
    unsigned char aesKey[AES_KEY_SIZE];  
    std::vector<fs::path> generatedFiles;  
    void handleErrors();  

public:
    explicit Crypto(const std::vector<unsigned char>& key);
    static std::vector<unsigned char> deriveKeyFromPasswordAndSalt(const std::string& password, const std::vector<unsigned char>& salt, int iterations = 100000);    
    
    fs::path encrypt(const fs::path& inputFilePath);
    fs::path decrypt(const fs::path& inputFilePath);

    void cleanupFiles();  
};

#endif 
