#include "../include/manager.h"
#include "../include/fileManager.h"
#include "../include/crypto.h"
#include "../include/utils.h"
#include "../include/logger.h"
#include "../include/vaultManager.h"
Config& Config::getInstance(){
    static Config instance;
    return instance;
}
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

bool Manager::initialize()
{
    VaultManager& vaultManager = VaultManager::getInstance();
    vaultManager.initialize();
    return true;
}
Manager& Manager::getInstance(){
    static Manager instance;
    return instance;
}
bool Manager::userPasswordValidation(std::string password){
    this->key = Crypto::deriveKeyFromPasswordAndSalt(password, Config::getInstance().salt);
    return Manager::loadMetadataEncrypted();
}

bool Manager::saveMetadataEncrypted(const VaultMetadata metadata) {
    fs::path applicationDataPath = FileManager::getSpecialFolderPath(FOLDERID_ProgramData);
    fs::path vaultPath = applicationDataPath / "Vault";
    fs::path tempFile = vaultPath / "metadata.temp";
    fs::path encryptedFile = vaultPath / "metadata.vaultenc";

    try {
        std::ofstream ofs(tempFile, std::ios::binary | std::ios::trunc);
        Serialization::serializeVaultMetadata(ofs, metadata);
        ofs.close();

        Crypto crypto(Manager::getInstance().key);
        fs::path encryptedTemp = crypto.encrypt(tempFile);

        fs::rename(encryptedTemp, encryptedFile);

        fs::remove(tempFile);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to save encrypted metadata: " << e.what() << '\n';
        return false;
    }
}


bool Manager::loadMetadataEncrypted() {
    fs::path applicationDataPath = FileManager::getSpecialFolderPath(FOLDERID_ProgramData);
    fs::path vaultPath = applicationDataPath / "Vault";
    fs::path encryptedFile=vaultPath / "metadata.vaultenc";

    try {
        Crypto crypto(this->key);
        fs::path decryptedFile = crypto.decrypt(encryptedFile);

        std::ifstream ifs(decryptedFile, std::ios::binary);
        VaultMetadata metadata = Serialization::deserializeVaultMetadata(ifs);
        VaultManager::getInstance().setVaultMetadata(metadata);
        ifs.close();

        crypto.cleanupFiles(); 

        return true;
    } catch (...) {
        return false;
    }
}



