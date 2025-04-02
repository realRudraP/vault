#ifndef METADATA_MANAGER_H
#define METADATA_MANAGER_H

#include <string>

class MetadataManager {
public:
    static void saveMetadata(const std::string& filename, const std::string& dirPath, int chunkID);
};

#endif
