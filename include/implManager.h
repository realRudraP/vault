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
    std::vector<fs::path> splitFile(fs::path encryptedFilePath, int numParts);
    //fs::path generateRandomDirectory();

private: 
    std::vector<fs::path> splitFileDirs;
    fs::path generateRandomDirectory();
};

class Reconstructor{
public: 
    Reconstructor(const std::vector<fs::path> metadata);
    fs::path reconstruct();

private:
    fs::path reconstructedFilePath;
    
};

#endif
