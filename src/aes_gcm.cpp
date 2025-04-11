#include "../include/encmod.h"
#include <iostream>

#define CHUNK_SIZE (4 * 1024 * 1024)  // 4 MB

namespace fs = std::filesystem;

AES_GCM::AES_GCM(const std::vector<unsigned char>& key) {
    if (key.size() != AES_KEY_SIZE)
        throw std::runtime_error("Invalid AES key size");
    std::copy(key.begin(), key.end(), aesKey);
}

AES_GCM::~AES_GCM() {
    try {
        cleanupFiles();
        OPENSSL_cleanse(aesKey, AES_KEY_SIZE);
    } catch (const std::exception& e) {
        std::cerr << "Destructor Cleanup Error: " << e.what() << std::endl;
    }
}

void AES_GCM::handleErrors() {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("OpenSSL encountered an error.");
}

void AES_GCM::cleanupFiles() {
    for (const auto& file : generatedFiles) {
        try {
            if (fs::exists(file)) {
                fs::remove(file);
            }
        } catch (const std::exception& e) {
            std::cerr << "File Cleanup Error: " << e.what() << std::endl;
        }
    }
    generatedFiles.clear();
}

fs::path AES_GCM::encrypt(const fs::path& inputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile) throw std::runtime_error("Failed to open input file.");

    std::vector<unsigned char> iv(IV_SIZE);
    if (RAND_bytes(iv.data(), iv.size()) != 1) handleErrors();

    fs::path outputFilePath = inputFilePath.parent_path() / (inputFilePath.stem().string() + "-encrypt" + inputFilePath.extension().string());
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile) throw std::runtime_error("Failed to create output file.");

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) handleErrors();
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, nullptr) != 1) handleErrors();
    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, aesKey, iv.data()) != 1) handleErrors();

    outputFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());

    std::vector<unsigned char> inBuf(CHUNK_SIZE);
    std::vector<unsigned char> outBuf(CHUNK_SIZE + EVP_CIPHER_block_size(EVP_aes_256_gcm()));
    int outLen;

    while (inputFile.read(reinterpret_cast<char*>(inBuf.data()), CHUNK_SIZE) || inputFile.gcount() > 0) {
        std::streamsize bytesRead = inputFile.gcount();
        if (EVP_EncryptUpdate(ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(bytesRead)) != 1)
            handleErrors();
        outputFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);
    }

    if (EVP_EncryptFinal_ex(ctx, outBuf.data(), &outLen) != 1)
        handleErrors();
    outputFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);

    std::vector<unsigned char> tag(TAG_SIZE);
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag.data()) != 1)
        handleErrors();
    outputFile.write(reinterpret_cast<char*>(tag.data()), tag.size());

    EVP_CIPHER_CTX_free(ctx);
    inputFile.close();
    outputFile.close();

    generatedFiles.push_back(outputFilePath);
    return outputFilePath;
}

fs::path AES_GCM::decrypt(const fs::path& inputFilePath) {
    std::ifstream inputFile(inputFilePath, std::ios::binary);
    if (!inputFile) throw std::runtime_error("Failed to open input file.");

    inputFile.seekg(0, std::ios::end);
    std::streamoff fileSize = inputFile.tellg();
    if (fileSize < (IV_SIZE + TAG_SIZE)) throw std::runtime_error("File too small or corrupted.");

    std::vector<unsigned char> iv(IV_SIZE);
    inputFile.seekg(0, std::ios::beg);
    inputFile.read(reinterpret_cast<char*>(iv.data()), IV_SIZE);

    std::vector<unsigned char> tag(TAG_SIZE);
    inputFile.seekg(fileSize - TAG_SIZE, std::ios::beg);
    inputFile.read(reinterpret_cast<char*>(tag.data()), TAG_SIZE);

    inputFile.seekg(IV_SIZE, std::ios::beg);
    std::streamoff dataSize = fileSize - IV_SIZE - TAG_SIZE;

    fs::path outputFilePath = inputFilePath.parent_path() / (inputFilePath.stem().string() + "-decrypt" + inputFilePath.extension().string());
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile) throw std::runtime_error("Failed to create output file.");

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) handleErrors();

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) handleErrors();
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, nullptr) != 1) handleErrors();
    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, aesKey, iv.data()) != 1) handleErrors();

    std::vector<unsigned char> inBuf(CHUNK_SIZE);
    std::vector<unsigned char> outBuf(CHUNK_SIZE + EVP_CIPHER_block_size(EVP_aes_256_gcm()));
    int outLen;

    std::streamoff bytesRemaining = dataSize;
    while (bytesRemaining > 0) {
        std::streamsize toRead = static_cast<std::streamsize>(std::min<std::streamoff>(CHUNK_SIZE, bytesRemaining));
        inputFile.read(reinterpret_cast<char*>(inBuf.data()), toRead);
        std::streamsize bytesRead = inputFile.gcount();
        bytesRemaining -= bytesRead;

        if (EVP_DecryptUpdate(ctx, outBuf.data(), &outLen, inBuf.data(), static_cast<int>(bytesRead)) != 1)
            handleErrors();

        outputFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag.data()) != 1)
        handleErrors();

    if (EVP_DecryptFinal_ex(ctx, outBuf.data(), &outLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption failed: Tag mismatch or corrupted file.");
    }

    outputFile.write(reinterpret_cast<char*>(outBuf.data()), outLen);

    EVP_CIPHER_CTX_free(ctx);
    inputFile.close();
    outputFile.close();

    generatedFiles.push_back(outputFilePath);
    return outputFilePath;
}
