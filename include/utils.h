#ifndef VAULT_UTILS_H
#define VAULT_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include<random>
#include <openssl/rand.h>
#include <windows.h>

namespace fs = std::filesystem;

namespace Utilities {

    std::string takePwdFromUser(const std::string prompt);
    std::string takePwdOnce(const std::string prompt);
    std::vector<unsigned char> generateSalt(size_t length);
    std::vector<std::string> tokenize(const std::string& inputLine);
    bool getBoolFromUser(const std::string prompt, bool defaultValue = false);
    int getPositiveIntFromUser(const std::string prompt, int defaultValue = 0);
    void printHex(const std::vector<unsigned char>& data);
    void changeDirectory(fs::path path);
    std::string generateUUID();
    bool securelyDeleteFile(fs::path path, size_t numberOfPasses);
}

#endif // VAULT_UTILS_H
