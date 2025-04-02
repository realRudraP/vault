#ifndef VAULT_UTILS_H
#define VAULT_UTILS_H

#include<iostream>
#include<openssl/rand.h>
#include<windows.h>
#include<vector>

class Utilities{
    public:
    static std::string takePwdFromUser();
    static void securelyClearCharVector(std::vector<char>&);
    std::vector<unsigned char> generateSalt(size_t length);
};

#endif