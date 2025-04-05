#include "../include/manager.h"
#include "../include/fileManager.h"
#include "../include/crypto.h"
#include "../include/utils.h"
#include "../include/logger.h"
#include "../include/vaultManager.h"
Config::Config() { };
bool Config::loadConfig(fs::path vaultPath)
{
    std::string data;
    if (!FileManager::readFileContents(vaultPath, data)) {
        std::cerr << "Failed to read vault file." << std::endl;
        return false;
    }
    

    std::istringstream iss(data, std::ios::binary);

    size_t saltSize;
    try {
        iss.read(reinterpret_cast<char*>(&saltSize), sizeof(saltSize));
        if (iss.fail()) {
            std::cerr << "Failed to read salt size." << std::endl;
            return false;
        }

        salt.resize(saltSize);
        iss.read(reinterpret_cast<char*>(salt.data()), saltSize);

        iss.read(reinterpret_cast<char*>(&enableSecureDeletion), sizeof(enableSecureDeletion));
        if (iss.fail()) {
            std::cerr << "Failed to read secure deletion setting." << std::endl;
            return false;
        }
        iss.read(reinterpret_cast<char*>(&secureDeletionPasses), sizeof(secureDeletionPasses));
        if (iss.fail()) {
            std::cerr << "Failed to read secure deletion passes." << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return false;
    }
    return true;
}
bool Config::saveConfig()
{
    fs::path applicationDataPath = FileManager::getSpecialFolderPath(FOLDERID_ProgramData);
    fs::path vaultPath = applicationDataPath / "Vault";
    fs::path vaultFilePath = vaultPath / "vault";

    std::ostringstream oss(std::ios::binary);

    size_t saltSize = this->salt.size();
    oss.write(reinterpret_cast<const char*>(&saltSize), sizeof(saltSize));
    oss.write(reinterpret_cast<const char*>(this->salt.data()), saltSize);
    oss.write(reinterpret_cast<const char*>(&this->enableSecureDeletion), sizeof(this->enableSecureDeletion));
    oss.write(reinterpret_cast<const char*>(&this->secureDeletionPasses), sizeof(this->secureDeletionPasses));

    std::string data = oss.str();

    if (FileManager::createFileWithContents(vaultFilePath, data)) {
        LOG_INFO("Config saved successfully.");
        return true;
    } else {
        std::cerr << "Failed to save config." << std::endl;
        return false;
    }
}

Manager::Manager()
{
    VaultManager& vaultManager = VaultManager::getInstance();
    if(!vaultManager.initialize()){
        std::cerr << "Failed to initialize VaultManager." << std::endl;
        return;
    }
    
}


