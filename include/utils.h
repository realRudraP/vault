#ifndef VAULT_UTILS_H
#define VAULT_UTILS_H

#include<iostream>
#include<openssl/rand.h>
#include<windows.h>
#include<vector>
#include<sstream>
#include<iostream>
#include<string>
#include<iomanip>


namespace Utilities {
    std::string takePwdFromUser();
    void securelyClearCharVector(std::vector<char>&);
    std::vector<unsigned char> generateSalt(size_t length);
    std::vector<std::string> tokenize(const std::string &inputLine);
    bool getBoolFromUser(const std::string prompt, bool defaultValue = false);
    int getPositiveIntFromUser(const std::string prompt, int defaultValue = 0);
    void printHex(const std::vector<unsigned char>& data);
}

#endif