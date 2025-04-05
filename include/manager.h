#ifndef VAULT_CONTROL_H
#define VAULT_CONTROL_H
#include <Windows.h>
#include <filesystem>
#include <string>
#include <vector>
namespace fs = std::filesystem;

struct ChunkInfo {
    int order_index;
    std::filesystem::path chunk_path;
};
struct FileInfo {
    std::string filename;
    uint64_t file_size;
    std::vector<ChunkInfo> chunks;
};
class Config {
public:
    Config(fs::path vaultPath);
    Config();
    std::vector<unsigned char> salt;
    bool enableSecureDeletion;
    size_t secureDeletionPasses;
    bool saveConfig();
    bool loadConfig();
};
class Manager {
    void initVault();
    void loadExistingVault(const fs::path& vaultPath);
    void createNewVault(const fs::path& vaultPath);
    std::string password;

public:
    std::vector<unsigned char> key; // MAKE THIS PRIVATE
    Manager();
    Config config;
    void changeDirectory(fs::path path);
};

struct FileMetadata {
    std::string filename;
    uint64_t file_size;
    std::vector<ChunkInfo> chunks;
};

#endif