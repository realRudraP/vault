#ifndef IMPLMANAGER_H
#define IMPLMANAGER_H

#include<string>
#include<filesystem>
#include<fstream>
#include<vector>
#include<random>

namespace fs = std::filesystem;


class ImplManager{
public: 
    fs::path splitFile(fs::path encryptedFilePath, int numParts);
    fs::path reconstruct(fs::path sPathForChunks, int numParts);
    //fs::path generateRandomDirectory();

private: 
    std::vector<fs::path> splitFileDirs;
    fs::path reconstructedFilePath;
    fs::path generateRandomDirectory();
};


#endif
