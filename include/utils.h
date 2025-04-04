#ifndef VAULT_UTILS_H
#define VAULT_UTILS_H

#include<iostream>
#include<openssl/rand.h>
#include<windows.h>
#include<vector>
#include<sstream>
#include<iostream>
#include<string>


class Utilities{
    public:
    static std::string takePwdFromUser();
    static void securelyClearCharVector(std::vector<char>&);
    std::vector<unsigned char> generateSalt(size_t length);
    std::vector<std::string> tokenize(const std::string &inputLine);
    static bool getBoolFromUser(const std::string prompt, bool defaultValue = false);
    static int getPositiveIntFromUser(const std::string prompt, int defaultValue = 0);
};

#endif