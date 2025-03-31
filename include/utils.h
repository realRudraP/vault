#ifndef VAULT_UTILS_H
#define VAULT_UTILS_H

#include<iostream>
#include<windows.h>
#include<vector>

class Utilities{
    public:
    static std::vector<char> takePwdFromUser();
    static void securelyClearCharVector(std::vector<char>&);
};

#endif