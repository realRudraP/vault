#ifndef VAULT_CORE_H
#define VAULT_CORE_H

#include "implManager.h"
#include "manager.h"
class VaultMetadata{
    public:
        std::vector<fs::path> foldersToStore;
        std::vector<FileInfo> files;
};

class Core{
    bool splitFile(const fs::path& filePath, int numChunks);
    bool encryptFile(const fs::path& filePath, const std::vector<unsigned char>& key);
    bool decryptFile(const fs::path& filePath, const std::vector<unsigned char>& key);
    bool cleanupFiles(const std::vector<fs::path>& filesToDelete);
};

#endif