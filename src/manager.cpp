#include "../include/control.h"
#include "../include/fileManager.h"
#include "../include/utils.h"

Config::Config(std::vector<unsigned char> salt)
{
    this->salt = salt;
}
Config::Config() { 
    this->salt = Utilities().generateSalt(16);
    this->saveConfig();
};
bool Config::saveConfig(){
    fs::path applicationDataPath = FileManager().getSpecialFolderPath(FOLDERID_ProgramData);
    fs::path vaultPath = applicationDataPath / "Vault";
    fs::path vaultFilePath = vaultPath / "vault";
    if (FileManager().createFileWithContents(vaultFilePath, std::string(this->salt.begin(), this->salt.end())))
    {
        std::cout << "Config saved successfully." << std::endl;
        return true;
    }
    else
    {
        std::cout << "Failed to save config." << std::endl;
        return false;
    }
}

Manager::Manager()
{
    fs::path applicationDataPath = FileManager().getSpecialFolderPath(FOLDERID_ProgramData);
    fs::path vaultPath = applicationDataPath / "Vault";
    fs::path vaultFilePath = vaultPath / "vault";
    if (FileManager().checkFileExists(vaultFilePath))
    {
        config = Config(vaultFilePath);
    }
    else
    {
        std::cout<<"Welcome to Vault!"<<std::endl;
        std::cout<<"It looks like it's your first run of the app. Let us create a vault for you."<<std::endl;
        std::cout<<"Please enter a password for your vault: ";
        std::string pwd = Utilities::takePwdFromUser();
        std::cout<<"Please re-enter your password: ";
        std::string pwd2 = Utilities::takePwdFromUser();
        if (pwd != pwd2)
        {
            std::cout << "Passwords do not match. Exiting..." << std::endl;
            exit(1);
        }
        std::cout << "Creating vault..." << std::endl;

    }
}                