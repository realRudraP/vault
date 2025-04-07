#include "../include/manager.h"
//#include "../include/utils.h"
#include "../include/implManager.h"
//#include "../include/crypto.h"
#include "../include/logger.h"
#include <iostream>
/*int main(){
    SetConsoleOutputCP(CP_UTF8);
    Manager::getInstance().initialize();
    std::string inputLine;
    while(true){
        std::cout << "Vault> ";
        std::getline(std::cin, inputLine);
        std::cout<<"Got input: "<<inputLine << std::endl;
        std::vector<std::string> tokens = Utilities::tokenize(inputLine);
        if (tokens.empty()) {
            continue;
        }else if(tokens[0] == "exit"){
            std::cout << "Exiting Vault..." << std::endl;
            break;
        }else if(tokens[0]=="cd"){
            if (tokens.size() > 1) {
                fs::path newPath = fs::path(tokens[1]);
                Utilities::changeDirectory(newPath);
            } else {
                std::cout << "Usage: cd <directory>" << std::endl;
            }
        }else if(tokens[0]=="ls"||tokens[0]=="dir"){
            if (tokens.size()==2){
                fs::path path = fs::path(tokens[1]);
                if (fs::exists(path) && fs::is_directory(path)) {
                    for (const auto& entry : fs::directory_iterator(path)) {
                        std::cout << entry.path() << std::endl;
                    }
                } else {
                    std::cout << "Directory does not exist." << std::endl;
                }
            }else if(tokens.size()==1){
                fs::path currentPath = fs::current_path();
                for (const auto& entry : fs::directory_iterator(currentPath)) {
                    std::cout << entry.path() << std::endl;
                }
            }else{
                std::cout << "Usage: ls <directory>" << std::endl;
            }
        }
    }
}*/
    

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
