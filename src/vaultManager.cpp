#include "../include/vaultManager.h"

VaultManager::VaultManager()
{
    config = Config::getInstance();
}
VaultManager& VaultManager::getInstance()
{
    static VaultManager instance; // Guaranteed to be destroyed and instantiated on first use
    return instance;
}

bool VaultManager::initialize()
{
    fs::path vaultFilePath = FileManager::getSpecialFolderPath(FOLDERID_ProgramData) / "Vault" / "vault";
    if (!FileManager::checkFileExists(vaultFilePath)) {
        VaultManager::createNewVault(vaultFilePath);
    } else {
        VaultManager::loadExistingVault(vaultFilePath);
    }
    return true;
}

void VaultManager::setVaultMetadata(VaultMetadata data)
{
    this->vaultMetadata = data;
}

void VaultManager::createNewVault(fs::path vaultPath)
{
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
            ╔════════════════════════════════════════════════════════════════════╗
            ║              Secure Deletion Passes Configuration                  ║
            ╠════════════════════════════════════════════════════════════════════╣
            ║  You can choose how many times the file data should be             ║
            ║  overwritten before deletion.                                      ║
            ║                                                                    ║
            ║  More passes = Better security (on HDDs), but slower deletion.     ║
            ║                                                                    ║
            ║  Note for SSDs:                                                    ║
            ║  Due to wear leveling, SSDs may redirect overwrite operations to   ║
            ║  different physical locations. This means multiple passes may not  ║
            ║  reliably erase the original data.                                 ║
            ║                                                                    ║
            ║  For SSDs, it's recommended to use manufacturer-provided secure    ║
            ║  erase tools or full-disk encryption with secure key destruction.  ║
            ╚════════════════════════════════════════════════════════════════════╝
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
        { "Temp", FileManager::getSpecialFolderPath(FOLDERID_LocalAppData) / "Temp" },
        { "Public", FileManager::getSpecialFolderPath(FOLDERID_Public) },
        { "ProgramData", FileManager::getSpecialFolderPath(FOLDERID_ProgramData) },
        { "LocalAppData", FileManager::getSpecialFolderPath(FOLDERID_LocalAppData) },
        { "UserProfile", FileManager::getSpecialFolderPath(FOLDERID_Profile) },
        { "RecycleBinCandidate", FileManager::getSpecialFolderPath(FOLDERID_LocalAppData) / "Microsoft" / "Windows" / "INetCache" }
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
    if (!Manager::getInstance().saveMetadataEncrypted(vaultMetadata)) {
        std::cerr << "Error in saving metadata." << std::endl;
    }
}
void VaultManager::loadExistingVault(fs::path vaultPath)
{
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
                OPENSSL_cleanse(pwd.data(), pwd.length());
                exit(1);
            }
        }
    }

    OPENSSL_cleanse(pwd.data(), pwd.length());
}

bool VaultManager::executor(Command command)
{
    if (command.baseCommand == BaseCommand::HELP) {
        Manager::displayHelp();
        return true;
    } else if (command.baseCommand == BaseCommand::TRIVIAL) {
        return true;
    } else if (command.baseCommand == BaseCommand::EXIT) {
        std::cout << "Exiting gracefully" << std::endl;
        exit(0);
    } else if (command.baseCommand == BaseCommand::ADD) {
        std::cout << "Encrypting file..." << std::endl;
        Crypto cryp(Manager::getInstance().key);
        fs::path encPath = cryp.encrypt(command.filePath.value());
        LOG_WARN("Encrypted file path: " + encPath.string());
        std::cout << Color::GREEN << "Done\n"
                  << Color::RESET << "Chunking encrypted file.." << std::endl;
        fs::path chunkDir = ImplManager().splitFile(encPath, 10);
        std::cout << Color::GREEN << "Done\n"
                  << Color::RESET << "Scrambling across your filesystem.." << std::endl;
        FileInfo fileInfo;
        fileInfo.filename = command.internalName.value();
        fileInfo.file_size = fs::file_size(command.filePath.value());
        size_t index = 0;
        std::vector<fs::path> chunks;
        for (const auto& file : fs::directory_iterator(chunkDir)) {
            chunks.push_back(file.path());
        }
        std::sort(chunks.begin(), chunks.end(), [](const fs::path& a, const fs::path& b) {
            return a.filename().string() < b.filename().string();
        });
        try {
            for (const auto file : chunks) {
                fs::path randomFolder;
                size_t tries = 0;
                do {
                    randomFolder = VaultManager::getRandomFolder();
                    if (tries++ > 5) {
                        std::cerr << "Failed to find cover directories for 5 times. Please check your configuration." << std::endl;
                        throw std::runtime_error("Cover directories not found.");
                    }
                } while (!fs::exists(randomFolder));
                std::string randomName = Utilities::generateUUID() + ".vaultenc";
                fs::path newPath = randomFolder / randomName;
                fs::copy_file(file, newPath, fs::copy_options::overwrite_existing);
                ChunkInfo chunkInfo;
                chunkInfo.order_index = index++;
                chunkInfo.chunk_path = newPath;
                fileInfo.chunks.push_back(chunkInfo);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in adding file: " << e.what() << std::endl;
            std::cerr << Color::RED << "Cleaning up.." << std::endl;
            for (const auto ele : fileInfo.chunks) {
                Utilities::deleteFile(ele.chunk_path, command.secureDelete, config.secureDeletionPasses);
            }
            Utilities::deleteFile(encPath, command.secureDelete, config.secureDeletionPasses);
            Utilities::deleteFile(chunkDir, command.secureDelete, config.secureDeletionPasses);
            std::cerr << Color::YELLOW << "Done.\n Exiting without completing operation" << Color::RESET << std::endl;
            return false;
        }
        vaultMetadata.files.push_back(fileInfo);
        Manager::getInstance().saveMetadataEncrypted(vaultMetadata);
        std::cout << Color::GREEN << "Done\n"
                  << Color::RESET << "Cleaning up.." << std::endl;
        Utilities::deleteFile(command.filePath.value(), command.secureDelete, config.secureDeletionPasses);
        Utilities::deleteFile(encPath, command.secureDelete, config.secureDeletionPasses);
        Utilities::deleteFile(chunkDir, command.secureDelete, config.secureDeletionPasses);
        std::cout << Color::GREEN << "Done\n"
                  << Color::RESET << "Your file has been added to the vault!" << std::endl;
        return true;
    } else if (command.baseCommand == BaseCommand::FETCH) {

        std::cout << "Searching for file in database..." << std::endl;
        FileInfo storedFile;
        fs::path gluedTogetherFile;
        fs::path decryptedFile;
        bool found = false;
        for (const auto file : vaultMetadata.files) {
            if (file.filename == command.internalName.value()) {
                storedFile = file;
                found = true;
                break;
            }
        }
        if (!found) {
            std::cerr << Color::RED << "Error: File " << command.internalName.value() << " was not found in your vault." << Color::RESET << std::endl;
            return false;
        }
        std::cout << Color::GREEN << "Done.\n"
                  << Color::RESET << "Finding,collecting and sorting chunks.." << std::endl;
        std::vector<ChunkInfo> chunks = storedFile.chunks;

        std::sort(chunks.begin(), chunks.end(), [](const ChunkInfo& a, const ChunkInfo& b) {
            return a.order_index < b.order_index;
        });

        std::vector<fs::path> chunkPaths;
        chunkPaths.reserve(chunks.size());

        for (const auto& chunk : chunks) {
            chunkPaths.push_back(chunk.chunk_path);
        }
        std::cout << Color::GREEN << "Done\n"
                  << Color::RESET;
        std::cout << "Reconstructing encrypted file from chunks..." << std::endl;
        try {
            gluedTogetherFile = ImplManager().reconstruct(chunkPaths, 10, storedFile.filename);
            std::cout << Color::GREEN << "Done\n"
                      << Color::RESET;
            std::cout << "Decrypting file..." << std::endl;

            Crypto cry(Manager::getInstance().key);
            decryptedFile = cry.decrypt(gluedTogetherFile);

            std::cout << Color::GREEN << "Done\n"
                      << Color::RESET << "Cleaning up.." << std::endl;
            Utilities::deleteFile(gluedTogetherFile, command.secureDelete, config.secureDeletionPasses);
            LOG_WARN("Decrypted file path: " + decryptedFile.string());
            std::cout << Color::GREEN << "Done\n"
                      << Color::RESET;
            return true;
        } catch (std::exception& e) {
            std::cerr << Color::RED << "Error during file fetch: " << e.what() << Color::RESET << std::endl;
            std::cerr << Color::YELLOW << "Cleaning up intermediate files..." << Color::RESET << std::endl;
            if (!gluedTogetherFile.empty() && fs::exists(gluedTogetherFile)) {
                Utilities::deleteFile(gluedTogetherFile, command.secureDelete, config.secureDeletionPasses);
            }
            if (!decryptedFile.empty() && fs::exists(decryptedFile)) {

                Utilities::deleteFile(decryptedFile, command.secureDelete, config.secureDeletionPasses);
            }
            std::cerr << Color::GREEN << "Done.\n"
                      << Color::YELLOW << "Exiting without completing operation." << Color::RESET << std::endl;
            return false;
        }
    } else if (command.baseCommand == BaseCommand::ENCRYPT) {
        try {
            std::cout << "Encrypting file..." << std::endl;
            Crypto cry(Manager::getInstance().key);
            fs::path encFile = cry.encrypt(command.filePath.value());
            fs::path interName(command.internalName.value());
            fs::copy_file(encFile, interName);
            std::cout << Color::GREEN << "Done\n"
                      << Color::RESET << "Cleaning up.." << std::endl;

            Utilities::deleteFile(encFile, command.secureDelete, config.secureDeletionPasses);
            std::cout << Color::GREEN << "Done\n"
                      << Color::RESET;
            return true;
        } catch (std::exception& e) {
            std::cout << e.what();
            return false;
        }
    } else if (command.baseCommand == BaseCommand::DECRYPT) {
        std::cout << "Decrypting file..." << std::endl;
        Crypto cry(Manager::getInstance().key);
        fs::path decFile = cry.decrypt(command.filePath.value());
        std::cout << Color::GREEN << "Done\n"
                      << Color::RESET << "Cleaning up.." << std::endl;
        fs::copy_file(decFile, decFile.parent_path() / decFile.stem());
        std::cout << "Done" << std::endl;
        return true;
    } else if (command.baseCommand == BaseCommand::LIST) {
        this->printStoredFiles();
        return true;
    } else if (command.baseCommand == BaseCommand::INVALID) {
        std::cout << "Invalid command entered. Use 'help' to find list of valid commands" << std::endl;
        return true;
    }else if(command.baseCommand==BaseCommand::SDELETE){
        Utilities::deleteFile(command.filePath.value(), true, 10);
        return true;
    }
    return false;
}
fs::path VaultManager::getRandomFolder()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, vaultMetadata.foldersToStore.size() - 1);
    return vaultMetadata.foldersToStore[dis(gen)];
}

void VaultManager::printStoredFiles()
{
    std::cout << "=== Stored Files ===\n";
    const auto files = this->vaultMetadata.files;
    if (files.empty()) {
        std::cout << "No files stored.\n";
    } else {
        for (const auto& file : files) {
            std::cout << "- " << file.filename << "\n";
        }
    }
    std::cout << "====================\n";
}
