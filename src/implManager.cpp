#include "../include/implManager.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include<filesystem>

namespace fs = std::filesystem;

std::vector<fs::path> ImplManager::splitFile(fs::path encryptedFilePath, int numChunks) {
    fs::path sFilePath = encryptedFilePath;
    std::cout << "🔹 Received file path: " << sFilePath << std::endl;

    if (!fs::exists(sFilePath)) {
        std::cerr << "File does not exist!" << std::endl;
        return splitFileDirs;
    }

    std::ifstream sFile(sFilePath,std::ios::binary);
    if(!sFile.is_open()){
        std::cerr<<"Error opening Fil! "<<sFilePath<<std::endl;
    }


    //CALC. FILESIZES && CHUNK SIZES
    sFile.seekg(0, std::ios::end); 
    std::size_t fileSize = sFile.tellg(); 
    sFile.seekg(0, std::ios::beg); 


    std::size_t chunkSize = fileSize/numChunks;
    std::size_t lastChunkSize = chunkSize + (fileSize % numChunks);

    //WRITING CHUNKS INTO FILES
    char* buffer = new char[chunkSize];
    for(int i=0; i<numChunks; i++){
        fs::path chunkDir = generateRandomDirectory();
        fs::create_directory(chunkDir);

        fs::path chunkFile = chunkDir / ("chunk_" + std::to_string(i) + ".bin");
        std::ofstream outputFile(chunkFile, std::ios::binary);
        if(!outputFile.is_open()){
            std::cerr<<"Error creating output File"<<std::endl;
            return splitFileDirs;
        }

        std::size_t bytesToWrite = (i==numChunks-1) ? lastChunkSize : chunkSize;
        sFile.read(buffer, bytesToWrite);
        outputFile.write(buffer, sFile.gcount());
        std::cout << "Chunk " << i << " stored in: " << chunkDir << std::endl;
        splitFileDirs.push_back(chunkFile);

        outputFile.close();
    }
    delete[] buffer;
    sFile.close();

    return splitFileDirs; 
}

fs::path generateRandomDirectory(){
    static std::mt19937 rng(std::random_device{}()); 
    static std::uniform_int_distribution<int> dist(1000, 9999); 
    return fs::path("chunk_dir_" + std::to_string(dist(rng)));
}
