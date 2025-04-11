#include "../include/crypto.h"

Crypto::Crypto(const std::vector<unsigned char>& key) {
    if (key.size() != AES_KEY_SIZE) {
        throw std::runtime_error("Invalid AES key size");
    }
    std::copy(key.begin(), key.end(), aesKey);
}

void Crypto::handleErrors() {
    ERR_print_errors_fp(stderr);
    throw std::runtime_error("OpenSSL encountered an error.");
}

fs::path Crypto::encrypt(const fs::path& inputFilePath) {
    try {
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        if (!inputFile) throw std::runtime_error("Failed to open input file: " + inputFilePath.string());

        std::vector<unsigned char> plaintext((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
        inputFile.close();

        std::vector<unsigned char> iv(IV_SIZE);
        if (RAND_bytes(iv.data(), iv.size()) != 1) throw std::runtime_error("Failed to generate IV.");

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) handleErrors();

        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, aesKey, iv.data()) != 1) handleErrors();

        std::vector<unsigned char> ciphertext(plaintext.size());
        int out_len;

        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &out_len, plaintext.data(), plaintext.size()) != 1) handleErrors();
        int total_len = out_len;

        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + out_len, &out_len) != 1) handleErrors();
        total_len += out_len;

        std::vector<unsigned char> tag(TAG_SIZE);
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag.data()) != 1) handleErrors();

        EVP_CIPHER_CTX_free(ctx);

        fs::path outputFilePath = inputFilePath.parent_path() / (inputFilePath.stem().string() + "-encrypt" + inputFilePath.extension().string());
        std::ofstream outputFile(outputFilePath, std::ios::binary);
        if (!outputFile) throw std::runtime_error("Failed to create output file: " + outputFilePath.string());

        outputFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());
        outputFile.write(reinterpret_cast<const char*>(ciphertext.data()), total_len);
        outputFile.write(reinterpret_cast<const char*>(tag.data()), tag.size());
        outputFile.close();

        generatedFiles.push_back(outputFilePath);
        return outputFilePath;
    } catch (const std::exception& e) {
        throw e;
        return "";
    }
}

fs::path Crypto::decrypt(const fs::path& inputFilePath) {
    try {
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        if (!inputFile) throw std::runtime_error("Failed to open input file: " + inputFilePath.string());

        std::vector<unsigned char> encryptedData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
        inputFile.close();

        if (encryptedData.size() < IV_SIZE + TAG_SIZE) throw std::runtime_error("Invalid encrypted data size.");

        std::vector<unsigned char> iv(encryptedData.begin(), encryptedData.begin() + IV_SIZE);
        std::vector<unsigned char> tag(encryptedData.end() - TAG_SIZE, encryptedData.end());
        std::vector<unsigned char> ciphertext(encryptedData.begin() + IV_SIZE, encryptedData.end() - TAG_SIZE);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx) handleErrors();

        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, aesKey, iv.data()) != 1) handleErrors();

        std::vector<unsigned char> plaintext(ciphertext.size());
        int out_len;

        if (EVP_DecryptUpdate(ctx, plaintext.data(), &out_len, ciphertext.data(), ciphertext.size()) != 1) handleErrors();
        int total_len = out_len;

        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag.data()) != 1) handleErrors();

        if (EVP_DecryptFinal_ex(ctx, plaintext.data() + out_len, &out_len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Decryption failed: Invalid tag or corrupted data.");
        }
        total_len += out_len;
        plaintext.resize(total_len);

        EVP_CIPHER_CTX_free(ctx);

        fs::path outputFilePath = inputFilePath.parent_path() / (inputFilePath.stem().string() + "-decrypt" + inputFilePath.extension().string());
        std::ofstream outputFile(outputFilePath, std::ios::binary);
        if (!outputFile) throw std::runtime_error("Failed to create output file: " + outputFilePath.string());

        outputFile.write(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        outputFile.close();

        generatedFiles.push_back(outputFilePath);
        return outputFilePath;
    } catch (const std::exception& e) {
        throw e;
        return "";
    }
}

void Crypto::cleanupFiles() {
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

std::vector<unsigned char> Crypto::deriveKeyFromPasswordAndSalt(const std::string& password,const std::vector<unsigned char>& salt, int iterations ){
    std::vector<unsigned char> key(Crypto::AES_KEY_SIZE);
    if(PKCS5_PBKDF2_HMAC(
        password.c_str(),
        password.length(),
        salt.data(),
        salt.size(),
        iterations,
        EVP_sha256(),
        Crypto::AES_KEY_SIZE,
        key.data())!=1){
            throw std::runtime_error("Key derivation failed.");
        }
        return key;
}
std::vector<unsigned char> Crypto::fileSHA256Generator(fs::path filePath){
    const size_t bufferSize = 4096;
    std::vector<unsigned char> buffer(bufferSize);
    std::vector<unsigned char> hash(EVP_MAX_MD_SIZE);
    unsigned int hash_len = 0;

    std::ifstream inputFile(filePath, std::ios::binary);


    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    while(inputFile.good()){
        inputFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        std::streamsize bytesRead = inputFile.gcount();

        if(bytesRead>0){
            EVP_DigestUpdate(ctx, buffer.data(), bytesRead);
        }
        
    }
    EVP_DigestFinal_ex(ctx, hash.data(), &hash_len);
    EVP_MD_CTX_free(ctx);
    hash.resize(hash_len);
    return hash;
}