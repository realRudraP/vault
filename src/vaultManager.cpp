#include "../include/vaultManager.h"

VaultManager::VaultManager(){
    config = Config::getInstance();
}
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

void VaultManager::setVaultMetadata(VaultMetadata data){
    this->vaultMetadata = data;
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
    Manager::getInstance().userPasswordValidation(password);
    OPENSSL_cleanse(password.data(), password.length());
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
        std::cout << R"(
            ╔════════════════════════════════════════════════════════╗
            ║             Vault Storage Folder Setup                 ║
            ╠════════════════════════════════════════════════════════╣
            ║  Choose the folders where encrypted chunks of your     ║
            ║  vault will be stored.                                 ║
            ║                                                        ║
            ║  Hidden folders will be created inside each location.  ║
            ║  Choose wisely — multiple locations = more resilience. ║
            ║                                                        ║
            ║  Type the number to select a folder.                   ║
            ║  Type 'done' to finish selection.                      ║
            ╚════════════════════════════════════════════════════════╝
        )" << std::endl;
        
        std::vector<std::pair<std::string, fs::path>> defaultLocations = {
            {"Documents", FileManager::getSpecialFolderPath(FOLDERID_Documents)},
            {"Downloads", FileManager::getSpecialFolderPath(FOLDERID_Downloads)},
            {"Pictures",  FileManager::getSpecialFolderPath(FOLDERID_Pictures)},
            {"Videos",    FileManager::getSpecialFolderPath(FOLDERID_Videos)},
            {"Desktop",   FileManager::getSpecialFolderPath(FOLDERID_Desktop)},
            {"AppData",   FileManager::getSpecialFolderPath(FOLDERID_RoamingAppData)}
        };
        
        for (size_t i = 0; i < defaultLocations.size(); ++i) {
            std::cout << "  [" << i + 1 << "] " << defaultLocations[i].first 
                      << " (" << defaultLocations[i].second << ")\n";
        }
        std::cout << "  [7] Enter a custom absolute path\n";
        std::cout << "  [done] Finish setup\n\n";
        
        std::string input;
        while (true) {
            std::cout << "Enter your choice: ";
            std::getline(std::cin, input);
        
            if (input == "done") {
                if (vaultMetadata.foldersToStore.empty()) {
                    std::cout << "You must select at least one folder before finishing setup.\n";
                    continue;
                }
                break;
            }
        
            fs::path selectedPath;
            if (input == "7") {
                std::cout << "\nEnter full absolute path: ";
                std::getline(std::cin, input);
                selectedPath = fs::path(input);
            } else {
                int index;
                try {
                    index = std::stoi(input) - 1;
                } catch (...) {
                    std::cout << "Invalid input. Please enter a number or 'done'.\n";
                    continue;
                }
        
                if (index >= 0 && index < static_cast<int>(defaultLocations.size())) {
                    selectedPath = defaultLocations[index].second;
                } else {
                    std::cout << "Invalid choice. Try again.\n";
                    continue;
                }
            }
        
            if (!fs::exists(selectedPath)) {
                std::cout << "Path does not exist: " << selectedPath << "\n";
                continue;
            }
        
            try {
                fs::path hiddenVaultFolder = FileManager::createHiddenVaultFolder(selectedPath);
                vaultMetadata.foldersToStore.push_back(hiddenVaultFolder);
            } catch (const std::exception& e) {
                std::cout << " Failed to create hidden folder: " << e.what() << "\n";
            }
        }
        if(!Manager::getInstance().saveMetadataEncrypted(vaultMetadata)){
            std::cerr<<"Error in saving metadata."<<std::endl;
        }
}
void VaultManager::loadExistingVault(fs::path vaultPath) {
    config.loadConfig(vaultPath);
    std::cout << R"(
        ╔════════════════════════════════════════════════════════╗
        ║                    Welcome Back!                       ║
        ╠════════════════════════════════════════════════════════╣
        ║  We're glad to see you again. Your encrypted vault is  ║
        ║  ready and waiting. Let's unlock it so you can get     ║
        ║  back to your secure world.                            ║
        ║                                                        ║
        ║  Please enter your master password below.              ║
        ║                                                        ║
        ╚════════════════════════════════════════════════════════╝
    )" << std::endl;
    std::string pwd;
int attemptsLeft = 3;

while (attemptsLeft > 0) {
    pwd = Utilities::takePwdOnce("Enter your password");

    if (Manager::getInstance().userPasswordValidation(pwd)) {
        std::cout << "Access granted. Welcome back!\n";
        break;
    } else {
        --attemptsLeft;
        if (attemptsLeft > 0) {
            std::cout << "Incorrect password. You have " << attemptsLeft 
                      << " attempt" << (attemptsLeft == 1 ? "" : "s") << " left.\n";
        } else {
            std::cout << "Too many failed attempts. Exiting...\n";
            exit(1); 
        }
    }
}

    OPENSSL_cleanse(pwd.data(), pwd.length());
    
}