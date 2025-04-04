#ifndef VAULT_CONTROL_H
#define VAULT_CONTROL_H
#include<filesystem>
#include<string>
#include<Windows.h>
#include<vector>
namespace fs = std::filesystem;

struct ChunkInfo{
    int order_index;
    std::filesystem::path chunk_path;
};
class Config{
    public:
    Config(fs::path vaultPath);
    Config();
    std::vector<unsigned char> salt;
    bool enableSecureDeletion;
    size_t secureDeletionPasses;
    bool saveConfig();
    bool loadConfig();
};
class Manager{
    void initVault();
    void loadExistingVault(const fs::path& vaultPath);
    void createNewVault(const fs::path& vaultPath);
    public:
    Manager();
    Config config;
    void changeDirectory(fs::path path);
};

struct FileMetadata{
    std::string filename;
    uint64_t file_size;
    std::vector<ChunkInfo> chunks;
};

#endif