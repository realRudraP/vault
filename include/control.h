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
    Config(std::vector<unsigned char> salt);
    Config(fs::path vaultPath);
    Config();
    std::vector<unsigned char> salt;
    bool saveConfig();
    bool loadConfig();
};
class Manager{
    public:
    Manager();
    Config config;
};

struct FileMetadata{
    std::string filename;
    uint64_t file_size;
    std::vector<ChunkInfo> chunks;
};

#endif