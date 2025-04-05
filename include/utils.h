#ifndef VAULT_UTILS_H
#define VAULT_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <openssl/rand.h>
#include <windows.h>

namespace fs = std::filesystem;

namespace Utilities {

    std::string takePwdFromUser(const std::string prompt);
    void securelyClearCharVector(std::vector<char>&);
    std::vector<unsigned char> generateSalt(size_t length);
    std::vector<std::string> tokenize(const std::string& inputLine);
    bool getBoolFromUser(const std::string prompt, bool defaultValue = false);
    int getPositiveIntFromUser(const std::string prompt, int defaultValue = 0);
    void printHex(const std::vector<unsigned char>& data);
    void changeDirectory(fs::path path);

}

#endif // VAULT_UTILS_H
