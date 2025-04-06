#ifndef VAULT_MANAGER_H
#define VAULT_MANAGER_H

#include <Windows.h>
#include <filesystem>
#include <string>
#include <vector>
#include <optional>
#include "implManager.h"
#include "serialization.h"
namespace fs = std::filesystem;


struct ChunkInfo {
    int order_index;
    std::filesystem::path chunk_path;
};
struct FileInfo {
    std::string filename;
    uint64_t file_size;
    std::vector <unsigned char> hash;
    std::vector<ChunkInfo> chunks;
};
struct FileMetadata {
    std::string filename;
    uint64_t file_size;
    std::vector<ChunkInfo> chunks;
};
class VaultMetadata{
    public:
        std::vector<fs::path> foldersToStore;
        std::vector<FileInfo> files;
};
class Config {
    Config()=default;
    public:
        static Config& getInstance();
        std::vector<unsigned char> salt;
        bool enableSecureDeletion;
        size_t secureDeletionPasses;
        bool saveConfig();
        bool loadConfig(fs::path configPath);
};
class Manager {
    Manager()=default;
    std::vector<unsigned char> key;
    bool loadMetadataEncrypted();
public:
    static Manager& getInstance();
    bool initialize();
    void changeDirectory(fs::path path);
    bool userPasswordValidation(std::string password);
    bool saveMetadataEncrypted(const VaultMetadata metadata);
    
};



class Core{
    bool splitFile(const fs::path& filePath, int numChunks);
    bool encryptFile(const fs::path& filePath, const std::vector<unsigned char>& key);
    bool decryptFile(const fs::path& filePath, const std::vector<unsigned char>& key);
    bool cleanupFiles(const std::vector<fs::path>& filesToDelete);
};


#endif