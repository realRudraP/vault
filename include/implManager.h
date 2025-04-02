#ifndef IMPLMANAGER_H
#define IMPLMANAGER_H

#include<string>
#include<filesystem>
#include<fstream>
#include<vector>

namespace fs = std::filesystem;


class FileManager{
public: 
    FileManager(fs::path encryptedFilePath, int numParts);
    std::vector<fs::path> splitFile();

private: 
    std::vector<fs::path> splitFilePaths;

};

class Reconstructor{
public: 
    Reconstructor(const std::vector<fs::path> metadata);
    fs::path reconstruct();

private:
    fs::path reconstructedFilePath;
    
};

#endif
