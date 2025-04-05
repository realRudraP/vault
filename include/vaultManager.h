#ifndef VAULT_VAULTMANAGER_H
#define VAULT_VAULTMANAGER_H

#include <filesystem>
#include "fileManager.h"
#include "utils.h"
#include "manager.h"
#include "logger.h"

namespace fs = std::filesystem;

class VaultManager {
private:
    VaultManager() = default; 
    VaultManager(const VaultManager&) = delete;
    VaultManager& operator=(const VaultManager&) = delete;

    void loadExistingVault(fs::path vaultPath);
    void createNewVault(fs::path vaultPath);

public:
    static VaultManager& getInstance();

    Config config;
    
    bool initialize();
    bool isInitialized() const;
};

#endif
