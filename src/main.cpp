#include "../include/logger.h"
#include "../include/manager.h"
#include "../include/utils.h"
#include "../include/parser.h"
#include "../include/vaultManager.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(){
    SetConsoleOutputCP(CP_UTF8);
    Manager::getInstance().initialize();
    std::string inputLine;
    Parser parser;
    while(true){
        fs::path currentPath = fs::current_path();
        std::cout<< currentPath.generic_string() << ": Vault> ";
        std::getline(std::cin, inputLine);
        std::transform(inputLine.begin(), inputLine.end(), inputLine.begin(),
                   [](unsigned char c){ return std::tolower(c); });
        std::vector<std::string> tokens = Utilities::tokenize(inputLine);
        inputLine.clear();
        Command currentCommand = parser.parse(tokens);
        try{
            VaultManager::getInstance().executor(currentCommand);
        }catch (std::exception e){
            std::cerr << "Error in executing command: " << e.what() << std::endl;
        }
    }
}
