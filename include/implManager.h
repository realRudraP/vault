#ifndef IMPLMANAGER_H
#define IMPLMANAGER_H

#include<string>
#include "logger.h"
#include<filesystem>
#include<fstream>
#include<vector>
#include<random>

namespace fs = std::filesystem;


class ImplManager{
public: 
    fs::path splitFile(fs::path encryptedFilePath, int numParts);
    fs::path reconstruct(std::vector<fs::path> sPathForChunks, int numParts, std::string fileName);
    std::vector<fs::path> getFiles(const fs::path& dir);//fs::path generateRandomDirectory();

private: 
    std::vector<fs::path> splitFileDirs;
    fs::path reconstructedFilePath;
    fs::path generateRandomDirectory();
};


#endif
