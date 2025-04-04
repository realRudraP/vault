#include "../include/fileManager.h"

FileManager::FileManager() { };
bool FileManager::checkFileExists(const fs::path& filePath) {
    std::cout << "Entered check" << std::endl;
    return fs::exists(filePath);
}
bool FileManager::createFileWithContents(const fs::path& filePath, const std::string& contents) {
    // Ensure the parent directory exists
    fs::create_directories(filePath.parent_path());

    std::ofstream file(filePath);
    if (!file) {
        return false; 
    }
    file << contents;
    return true; 
}

bool FileManager::createFile(const fs::path& filePath) {
    // Ensure the parent directory exists
    fs::create_directories(filePath.parent_path());

    std::ofstream file(filePath);
    if (!file) {
        return false; 
    }
    return true; 
}


fs::path FileManager::getSpecialFolderPath(REFKNOWNFOLDERID folderId) {
    PWSTR path = nullptr;
    HRESULT hr = SHGetKnownFolderPath(folderId, 0, NULL, &path);
    if (SUCCEEDED(hr)) {
        fs::path result(path);
        CoTaskMemFree(path); 
        return result;
    }
    return fs::path(); 
}

bool FileManager::readFileContents(const std::filesystem::path& filePath, std::string& outContents) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate); 
    if (!file.is_open()) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    outContents.resize(size);
    if (!file.read(&outContents[0], size)) return false;

    return true;
}
