#include "../include/vaultManager.h"


VaultManager& VaultManager::getInstance() {
    static VaultManager instance; // Guaranteed to be destroyed and instantiated on first use
    return instance;
}

bool VaultManager::initialize() {
    fs::path vaultFilePath = FileManager::getSpecialFolderPath(FOLDERID_ProgramData) / "Vault" / "vault";
    if(!FileManager::checkFileExists(vaultFilePath)){
        VaultManager::createNewVault(vaultFilePath);
    }else{
        VaultManager::loadExistingVault(vaultFilePath);
    }
    return true;
}

void VaultManager::createNewVault(fs::path vaultPath) {
     // The R"" syntax is used 
    // for raw string literals, allowing to include special 
    // characters without escaping them.
    std::cout << R"(          
        ╔════════════════════════════════════════════════════════╗
        ║                Welcome to Vault Setup!                 ║
        ╠════════════════════════════════════════════════════════╣
        ║  Let's get you started with your personal, secure      ║
        ║  vault. All files you store here will be protected     ║
        ║  with AES encryption.                                  ║
        ║                                                        ║
        ║  ->  First, set a strong password. This will be used   ║
        ║  to lock and unlock your vault.                        ║
        ║                                                        ║
        ║  -> Make sure you remember this password — there's no  ║
        ║  "Forgot Password" option.                             ║
        ╚════════════════════════════════════════════════════════╝
        )" << std::endl;
    std::string password = Utilities::takePwdFromUser("Enter your password");
    std::cout << R"(
        ╔════════════════════════════════════════════════════════╗
        ║                Secure Deletion Options                 ║
        ╠════════════════════════════════════════════════════════╣
        ║  Secure Deletion ensures that when a file is deleted,  ║
        ║  its contents are overwritten — making recovery nearly ║
        ║  impossible. Ideal for sensitive or private data.      ║
        ║                                                        ║
        ║  Would you like to enable this feature?                ║
        ║  (This may slow down deletion slightly.)               ║
        ╚════════════════════════════════════════════════════════╝
        )" << std::endl;
        
        bool enableSecureDeletion = Utilities::getBoolFromUser("Enable secure deletion?", false);
        config.enableSecureDeletion = enableSecureDeletion;
        
        if (enableSecureDeletion) {
            std::cout << R"(
        ╔════════════════════════════════════════════════════════╗
        ║          Secure Deletion Passes Configuration          ║
        ╠════════════════════════════════════════════════════════╣
        ║  You can choose how many times the file data should be ║
        ║  overwritten before being deleted.                     ║
        ║                                                        ║
        ║  More passes = Better security, but slower deletion.   ║
        ║  Recommended: 3 passes for general use                 ║
        ╚════════════════════════════════════════════════════════╝
        )" << std::endl;
        
            int passes = Utilities::getPositiveIntFromUser("How many passes would you like?", 3);
            config.secureDeletionPasses = passes;
        } else {
            config.secureDeletionPasses = 0;
        }
        config.salt = Utilities::generateSalt(16);
        config.saveConfig();
        
}
void VaultManager::loadExistingVault(fs::path vaultPath) {
    config.loadConfig(vaultPath);
    LOG_INFO("Welcome back to Vault!");
    LOG_INFO("Salt: " + std::string(config.salt.begin(), config.salt.end()));
    LOG_INFO("Secure deletion: " + std::string(config.enableSecureDeletion ? "Enabled" : "Disabled"));
    LOG_INFO("Secure deletion passes: " + std::to_string(config.secureDeletionPasses));
}