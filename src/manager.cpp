#include "../include/manager.h"
#include "../include/fileManager.h"
#include "../include/crypto.h"
#include "../include/utils.h"
#include "../include/logger.h"
Config::Config() { };
Config::Config(fs::path vaultPath)
{
    std::string data;
    if (!FileManager().readFileContents(vaultPath, data)) {
        std::cerr << "Failed to read vault file." << std::endl;
        return;
    }
    

    std::istringstream iss(data, std::ios::binary);

    size_t saltSize;
    try {
        iss.read(reinterpret_cast<char*>(&saltSize), sizeof(saltSize));
        if (iss.fail()) {
            std::cerr << "Failed to read salt size." << std::endl;
            return;
        }

        salt.resize(saltSize);
        iss.read(reinterpret_cast<char*>(salt.data()), saltSize);

        iss.read(reinterpret_cast<char*>(&enableSecureDeletion), sizeof(enableSecureDeletion));
        if (iss.fail()) {
            std::cerr << "Failed to read secure deletion setting." << std::endl;
            return;
        }
        iss.read(reinterpret_cast<char*>(&secureDeletionPasses), sizeof(secureDeletionPasses));
        if (iss.fail()) {
            std::cerr << "Failed to read secure deletion passes." << std::endl;
            return;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        return;
    }
}
bool Config::saveConfig()
{
    fs::path applicationDataPath = FileManager().getSpecialFolderPath(FOLDERID_ProgramData);
    fs::path vaultPath = applicationDataPath / "Vault";
    fs::path vaultFilePath = vaultPath / "vault";

    std::ostringstream oss(std::ios::binary);

    size_t saltSize = this->salt.size();
    oss.write(reinterpret_cast<const char*>(&saltSize), sizeof(saltSize));
    oss.write(reinterpret_cast<const char*>(this->salt.data()), saltSize);
    oss.write(reinterpret_cast<const char*>(&this->enableSecureDeletion), sizeof(this->enableSecureDeletion));
    oss.write(reinterpret_cast<const char*>(&this->secureDeletionPasses), sizeof(this->secureDeletionPasses));

    std::string data = oss.str();

    if (FileManager().createFileWithContents(vaultFilePath, data)) {
        LOG_INFO("Config saved successfully.");
        return true;
    } else {
        std::cerr << "Failed to save config." << std::endl;
        return false;
    }
}

Manager::Manager()
{
    Manager::initVault();
}

void Manager::initVault()
{
    fs::path applicationDataPath = FileManager().getSpecialFolderPath(FOLDERID_ProgramData);
    fs::path vaultPath = applicationDataPath / "Vault";
    fs::path vaultFilePath = vaultPath / "vault";
    if (FileManager().checkFileExists(vaultFilePath)) {
        this->loadExistingVault(vaultFilePath);
    } else {
        this->createNewVault(vaultFilePath);
    }
    this->key=Crypto::deriveKeyFromPasswordAndSalt(this->password, this->config.salt);
    

}
void Manager::loadExistingVault(const fs::path& vaultFilePath)
{
    config = Config(vaultFilePath);
    LOG_INFO("Welcome back to Vault!");
    LOG_INFO("Salt: " + std::string(config.salt.begin(), config.salt.end()));
    LOG_INFO("Secure deletion: " + std::string(config.enableSecureDeletion ? "Enabled" : "Disabled"));
    LOG_INFO("Secure deletion passes: " + std::to_string(config.secureDeletionPasses));
}
void Manager::createNewVault(const fs::path& vaultFilePath)
{
    std::cout << "Welcome to Vault!" << std::endl;
    std::cout << "It looks like it's your first run of the app. Let us create a vault for you." << std::endl;
    std::cout << "Please enter a password for your vault: ";
    std::string pwd = Utilities().takePwdFromUser();
    std::cout << "Please re-enter your password: ";
    std::string pwd2 = Utilities().takePwdFromUser();
    if (pwd != pwd2) {
        std::cout << "Passwords do not match. Exiting..." << std::endl;
        exit(1);
    }
    bool enableSecureDeletion = Utilities().getBoolFromUser("Would you like to enable secure deletion? (default is false)", false);
    this->config.enableSecureDeletion = enableSecureDeletion;
    if (enableSecureDeletion) {
        int secureDeletionPasses = Utilities().getPositiveIntFromUser("How many passes for secure deletion? (default is 3)", 3);
        this->config.secureDeletionPasses = secureDeletionPasses;
    } else {
        this->config.secureDeletionPasses = 0;
    }
    this->config.salt = Utilities().generateSalt(16);
    if (this->config.saveConfig()) {
        std::cout << "Vault created successfully." << std::endl;
    } else {
        std::cout << "Failed to create vault." << std::endl;
        exit(1);
    }
}

void Manager::changeDirectory(fs::path path)
{
    std::cout << "Changing directory to: " << path << std::endl;
    try {
        fs::current_path(path);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error changing directory: " << e.what() << std::endl;
    }
}