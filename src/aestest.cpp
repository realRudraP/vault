#include "../include/encmod.h"
#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include<string>
std::vector<unsigned char> generateRandomKey(size_t keySize) {
    std::vector<unsigned char> key(keySize);
    if (RAND_bytes(key.data(), keySize) != 1) {
        throw std::runtime_error("Failed to generate random AES key.");
    }
    return key;
}

void printHex(const std::vector<unsigned char>& data) {
    for (unsigned char byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::dec << std::endl;
}
/*
int main() {
    try {
        std::string filePath;
        std::cout << "Enter the file path: ";
        std::getline(std::cin, filePath);

        std::vector<unsigned char> key = generateRandomKey(32);
        std::cout << "Generated AES Key: ";
        printHex(key);

        AES_GCM aesGcm(key);

        fs::path encryptedFile = aesGcm.encrypt(filePath);
        if (encryptedFile.empty()) {
            std::cerr << "Encryption failed." << std::endl;
            return 1;
        }
        std::cout << "Encrypted file: " << encryptedFile << std::endl;

        fs::path decryptedFile = aesGcm.decrypt(encryptedFile);
        if (decryptedFile.empty()) {
            std::cerr << "Decryption failed." << std::endl;
            return 1;
        }
        std::cout << "Decrypted file: " << decryptedFile << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
*/