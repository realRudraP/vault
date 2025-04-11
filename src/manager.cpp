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

Manager::~Manager() {
    OPENSSL_cleanse(this->key.data(), this->key.size());
}

void Manager::displayHelp(){
    
        
        std::cout << Color::BOLD << Color::BRIGHT_CYAN << "Vault Command Help:" << Color::RESET << "\n";
        std::cout << Color::BRIGHT_CYAN << "-------------------" << Color::RESET << "\n\n";
    
        
        std::cout << Color::BOLD << Color::YELLOW << "Vault Operations:" << Color::RESET << "\n";
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "add" << Color::RESET
                  << " " << Color::CYAN << "<file_path>" << Color::RESET
                  << " " << Color::MAGENTA << "[-name <internal_name>]" << Color::RESET
                  << " " << Color::MAGENTA << "[-sdel]" << Color::RESET << "\n";
        std::cout << "    Adds the specified file to the vault.\n";
        std::cout << "    " << Color::CYAN << "<file_path>" << Color::RESET << ": Path to the file on your system.\n";
        std::cout << "    " << Color::MAGENTA << "-name <internal_name>" << Color::RESET << ": (Optional) Store the file under a different name inside the vault.\n";
        std::cout << "                         Defaults to the original filename.\n";
        std::cout << "    " << Color::MAGENTA << "-sdel" << Color::RESET << ": (Optional) Securely delete the original file after adding it.\n\n";
    
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "fetch" << Color::RESET
                  << " " << Color::CYAN << "<internal_name>" << Color::RESET << "\n";
        std::cout << "    Retrieves a file from the vault by its internal name.\n";
        std::cout << "    " << Color::CYAN << "<internal_name>" << Color::RESET << ": The name of the file as stored in the vault.\n";
        std::cout << "    The file will be saved to the current working directory.\n\n";
    
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "list" << Color::RESET << "\n";
        std::cout << "    Lists all files currently stored inside the vault.\n\n";
    
    
        
        std::cout << Color::BOLD << Color::YELLOW << "File Security Operations:" << Color::RESET << "\n";
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "sdelete" << Color::RESET
                  << " " << Color::CYAN << "<file_path>" << Color::RESET << "\n";
        std::cout << "    Securely deletes the specified file from your filesystem.\n";
        std::cout << "    " << Color::CYAN << "<file_path>" << Color::RESET << ": Path to the file on your system to securely delete.\n";
        std::cout << "    " << Color::BOLD << Color::BRIGHT_RED << "WARNING: This action is irreversible." << Color::RESET << "\n\n";
    
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "encrypt" << Color::RESET
                  << " " << Color::CYAN << "<file_path>" << Color::RESET << "\n";
        std::cout << "    Encrypts the specified file.\n";
        std::cout << "    " << Color::CYAN << "<file_path>" << Color::RESET << ": Path to the file on your system to encrypt.\n\n";
    
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "decrypt" << Color::RESET
                  << " " << Color::CYAN << "<file_path>" << Color::RESET << "\n";
        std::cout << "    Decrypts the specified file.\n";
        std::cout << "    " << Color::CYAN << "<file_path>" << Color::RESET << ": Path to the file on your system to decrypt.\n\n";
    
    
        
        std::cout << Color::BOLD << Color::YELLOW << "Filesystem Navigation (Standard Commands):" << Color::RESET << "\n";
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "ls" << Color::RESET
                  << " " << Color::MAGENTA << "[<directory_path>]" << Color::RESET << "\n";
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "dir" << Color::RESET
                  << " " << Color::MAGENTA << "[<directory_path>]" << Color::RESET << "\n";
        std::cout << "    Lists the contents of the specified directory or the current directory.\n\n";
    
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "cd" << Color::RESET
                  << " " << Color::CYAN << "<directory_path>" << Color::RESET << "\n";
        std::cout << "    Changes the current working directory.\n\n";
    
    
        
        std::cout << Color::BOLD << Color::YELLOW << "Application Control:" << Color::RESET << "\n";
        
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "help" << Color::RESET << "\n";
        std::cout << "    Displays this help message.\n\n";
    
        // exit command
        std::cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "exit" << Color::RESET << "\n";
        std::cout << "    Exits the vault application.\n\n";
    
        
        std::cout << Color::RESET;
    
}

