#ifndef VAULT_FILEMANAGER_H
#define VAULT_FILEMANAGER_H
#include<filesystem>
#include<string>
#include<vector>
#include<Windows.h>
#include<Shlobj.h>
#include<iostream>
#include "vaultManager.h"
#include<fstream>
#include<fstream>

namespace fs = std::filesystem;

namespace FileManager {
    fs::path getSpecialFolderPath(REFKNOWNFOLDERID folderId);
    bool checkFileExists(const fs::path& filePath);
    bool createFile(const fs::path& filePath);
    bool createFileWithContents(const fs::path& filePath, const std::string& contents);
    bool readFileContents(const fs::path& filePath, std::string& outContents);
    void hideFolder(const fs::path& folderPath);
    fs::path createHiddenVaultFolder(const fs::path parentDirectory);
}

namespace ProtectedFileManager{

};
#endif