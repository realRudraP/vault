#ifndef VAULT_VAULTMANAGER_H
#define VAULT_VAULTMANAGER_H

#include <filesystem>
#include "fileManager.h"
#include "utils.h"
#include "manager.h"
#include "logger.h"
#include "crypto.h"
#include "parser.h"
#include <openssl/crypto.h>

namespace fs = std::filesystem;

class VaultManager {
private:
    VaultManager(); 
    VaultManager(const VaultManager&) = delete;
    VaultManager& operator=(const VaultManager&) = delete;

    void loadExistingVault(fs::path vaultPath);
    void createNewVault(fs::path vaultPath);
    fs::path getRandomFolder();

public:
    static VaultManager& getInstance();
    VaultMetadata vaultMetadata;
    Config config=Config::getInstance();
    void setVaultMetadata(VaultMetadata data);
    bool initialize();
    bool isInitialized() const;
    bool executor(Command command);
    void printStoredFiles();
};

#endif
