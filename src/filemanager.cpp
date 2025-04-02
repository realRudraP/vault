#include "../include/fileManager.h"

FileManager::FileManager() { };
bool FileManager::checkFileExists(const fs::path& filePath) {
    return fs::exists(filePath);
}
bool FileManager::createFileWithContents(const fs::path& filePath, const std::string& contents) {
    // Ensure the parent directory exists
    fs::create_directories(filePath.parent_path());

    std::ofstream file(filePath);
    if (!file) {
        return false; // Failed to create the file
    }
    file << contents;
    return true; // File created successfully
}

bool FileManager::createFile(const fs::path& filePath) {
    // Ensure the parent directory exists
    fs::create_directories(filePath.parent_path());

    std::ofstream file(filePath);
    if (!file) {
        return false; // Failed to create the file
    }
    return true; // File created successfully
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
