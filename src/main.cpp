
#include "../include/crypto.h"
#include "../include/implManager.h"
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
        Command currentCommand = parser.parse(tokens);
        if(!VaultManager::getInstance().executor(currentCommand)){
            std::cerr << "Error ho gaya bhai" << std::endl;
        }
    }
}

/*
int main(){
    ImplManager implManager;
    //fs::path encryptedFilePath = "C:\\Users\\rudra\\Downloads\\image.jpg"; // Replace with your file path
    fs::path encryptedFilePath = "C:\\Users\\saman\\Downloads\\milky-way-2695569_1920.jpg"; // Replace with your file path
    int numChunks = 10; // Number of chunks to split the file into
    std::string fileName = "reconstructedFile.jpeg"; // Name of the reconstructed file

    // std::vector<fs::path> chunkDirs = implManager.splitFile(encryptedFilePath, numChunks);
    // for (const auto& dir : chunkDirs) {
    //     std::cout << "Chunk directory: " << dir << std::endl;
    // }

    fs::path splitFilePath = implManager.splitFile(encryptedFilePath, numChunks);
    std::vector<std::filesystem::path> holder = implManager.getFiles(splitFilePath);
    fs::path reconstructeedFilePath = implManager.reconstruct(holder, numChunks, fileName);

    std::cout<<"Reconstructed file path: "<<reconstructeedFilePath<<std::endl;


    

    return 0; 
}
*/