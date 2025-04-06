#ifndef VAULT_SER_H
#define VAULT_SER_H
#include <string>
#include <ostream>
#include <filesystem>
#include <vector>


class VaultMetadata;
namespace fs = std::filesystem;
namespace Serialization {
    void serializeString(std::ostream& os, const std::string& str);
    void serializePath(std::ostream& os, const std::filesystem::path& path);
    void serializeVectorUChar(std::ostream& os, const std::vector<unsigned char>& vec);
    void serializeVaultMetadata(std::ostream& os, const VaultMetadata& meta);

    std::string deserializeString(std::istream& is);
    std::filesystem::path deserializePath(std::istream& is);
    std::vector<unsigned char> deserializeVectorUChar(std::istream& is);
    VaultMetadata deserializeVaultMetadata(std::istream& is);
}

#endif