#include "../include/implManager.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include<filesystem>

namespace fs = std::filesystem;

fs::path ImplManager::splitFile(fs::path encryptedFilePath, int numChunks) {
    fs::path sFilePath = encryptedFilePath;
    std::cout << "[SPLITTING FUNCTION] Received file path: " << sFilePath << std::endl;

    if (!fs::exists(sFilePath)) {
        std::cerr << "File does not exist!" << std::endl;
        return sFilePath;
    }

    std::ifstream sFile(sFilePath, std::ios::binary);
    if (!sFile.is_open()) {
        std::cerr << "Error opening file: " << sFilePath << std::endl;
        return sFilePath;                
    }


    try {
        // Calculate file and chunk sizes
        sFile.seekg(0, std::ios::end);
        std::size_t fileSize = sFile.tellg();
        sFile.seekg(0, std::ios::beg);

        std::size_t chunkSize = fileSize / numChunks;
        std::size_t lastChunkSize = chunkSize + (fileSize % numChunks);

        // Create buffer using vector for automatic memory management
        // They also eliminate the need for manual memory management and potential memory leaks
        std::vector<char> buffer(std::max(chunkSize, lastChunkSize));

        //Updated code for a single directory
        fs::path outputDir = "chunks_output" ;
        if(!fs::exists(outputDir)){
            if(!fs::create_directory(outputDir)){
                throw std::runtime_error("Failed to create output directory: " + outputDir.string());
            }
        }
        /*std::vector<fs::path> createdDirs;*/

        for (int i = 0; i < numChunks; i++) {
            /*fs::path chunkDir = ImplManager::generateRandomDirectory();
            std::cout << "Chunk directory: " << chunkDir << std::endl;
            // Track created directories for cleanup in case of failure and to return to the orchestrator later on
            if (!fs::create_directory(chunkDir))
            {
                throw std::runtime_error("Failed to create directory: " + chunkDir.string());
            }
            createdDirs.push_back(chunkDir);*/

            fs::path chunkFile = outputDir / ("chunk_" + std::to_string(i) + ".bin");
            std::ofstream outputFile(chunkFile, std::ios::binary);
            
            if (!outputFile.is_open()) {
                throw std::runtime_error("Failed to create output file: " + chunkFile.string());
            }

            std::size_t bytesToWrite = (i == numChunks - 1) ? lastChunkSize : chunkSize;
            sFile.read(buffer.data(), bytesToWrite);
            
            if (sFile.fail() && !sFile.eof()) {
                throw std::runtime_error("Failed to read from input file");
            }

            outputFile.write(buffer.data(), sFile.gcount());
            
            if (outputFile.fail()) {
                throw std::runtime_error("Failed to write to output file: " + chunkFile.string());
            }

            std::cout << "Chunk " << i << " stored in: " << chunkFile << std::endl;
            //splitFileDirs.push_back(chunkFile);
            outputFile.close();
        }

        sFile.close();
        return outputDir;

    } catch (const std::exception& e) {
        std::cerr << "Error during file splitting: " << e.what() << std::endl;
        
        
        for (const auto& path : splitFileDirs) {
            try {
                fs::remove_all(path.parent_path());
            } catch (...) {
                std::cerr << "Failed to cleanup directory: " << path.parent_path() << std::endl;
            }
        }

        splitFileDirs.clear();
        sFile.close();
        return fs::path();
    }
}



fs::path ImplManager::reconstruct(std::vector<fs::path> sPath, int numParts, std::string fileName) {
    
    //LOG_INFO("[RECONSTRUCTING FUNCTION] Received vector path: " + to_string(sPath));

    if (sPath.empty() || !fs::exists(sPath[0])) {
        std::cerr << "Source does not exist!" << std::endl;
        return fs::path();
    }
    
    try {
        // Define the output file path
        fs::path reconstructedFilePath = fileName;

        std::ofstream outputFile(reconstructedFilePath, std::ios::binary);
        if (!outputFile.is_open()) {
            std::cerr << "Error opening output file stream: " << reconstructedFilePath << std::endl;
            return fs::path();
        }

        for (int i = 0; i < numParts; i++) {
            fs::path chunkFile = sPath[i]; // ("chunk_" + std::to_string(i) + ".bin");
            std::ifstream inputFile(chunkFile, std::ios::binary);

            if (!inputFile.is_open()) {
                std::cerr << "Error opening chunk file stream: " << chunkFile << std::endl;
                return fs::path();
            }

            inputFile.seekg(0, std::ios::end);
            std::size_t chunkSize = inputFile.tellg();
            inputFile.seekg(0, std::ios::beg);

            std::vector<char> buffer(chunkSize);
            inputFile.read(buffer.data(), chunkSize);

            if (inputFile.fail()) {
                std::cerr << "Error reading from chunk file: " << chunkFile << std::endl;
                return fs::path();
            }

            outputFile.write(buffer.data(), chunkSize);

            if (outputFile.fail()) {
                std::cerr << "Error writing to output file: " << reconstructedFilePath << std::endl;
                return fs::path();
            }

            inputFile.close();
        }

        outputFile.close();
        return reconstructedFilePath;
    } catch (const std::exception &e) {
        std::cerr << "Error during file reconstruction: " << e.what() << std::endl;
        return fs::path();
    }


}

std::vector<fs::path> ImplManager::getFiles(const fs::path& dir) {
        std::vector<fs::path> result;
        try{

            for (const auto& entry : fs::directory_iterator(dir)) {
                if (fs::is_regular_file(entry))
                    result.push_back(entry.path());
            }
        
            //std::sort(result.begin(), result.end()); // Ensure correct order!
            return result;
    
        } catch(const std::exception& e) {
            std::cerr << "Error reading directory[getFiles]: " << e.what() << std::endl;
            return std::vector<fs::path>();
        }
    }
    
    