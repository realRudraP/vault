#include "../include/serialization.h"
#include "../include/manager.h"

namespace Serialization {

    void serializeString(std::ostream& os, const std::string& str) {
        size_t len = str.size();
        os.write(reinterpret_cast<const char*>(&len), sizeof(len));
        os.write(str.data(), len);
    }

    void serializePath(std::ostream& os, const fs::path& path) {
        serializeString(os, path.string());
    }

    void serializeVectorUChar(std::ostream& os, const std::vector<unsigned char>& vec) {
        size_t size = vec.size();
        os.write(reinterpret_cast<const char*>(&size), sizeof(size));
        os.write(reinterpret_cast<const char*>(vec.data()), size);
    }

    void serializeVaultMetadata(std::ostream& os, const VaultMetadata& metadata) {
        size_t folderCount = metadata.foldersToStore.size();
        os.write(reinterpret_cast<const char*>(&folderCount), sizeof(folderCount));
        for (const auto& folder : metadata.foldersToStore)
            serializePath(os, folder);

        size_t fileCount = metadata.files.size();
        os.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));
        for (const auto& file : metadata.files) {
            serializeString(os, file.filename);
            os.write(reinterpret_cast<const char*>(&file.file_size), sizeof(file.file_size));
            serializeVectorUChar(os, file.hash);

            size_t chunkCount = file.chunks.size();
            os.write(reinterpret_cast<const char*>(&chunkCount), sizeof(chunkCount));
            for (const auto& chunk : file.chunks) {
                os.write(reinterpret_cast<const char*>(&chunk.order_index), sizeof(chunk.order_index));
                serializePath(os, chunk.chunk_path);
            }
        }
    }

    std::string deserializeString(std::istream& is) {
        size_t len;
        is.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string str(len, '\0');
        is.read(&str[0], len);
        return str;
    }

    fs::path deserializePath(std::istream& is) {
        return fs::path(deserializeString(is));
    }

    std::vector<unsigned char> deserializeVectorUChar(std::istream& is) {
        size_t size;
        is.read(reinterpret_cast<char*>(&size), sizeof(size));
        std::vector<unsigned char> vec(size);
        is.read(reinterpret_cast<char*>(vec.data()), size);
        return vec;
    }

    VaultMetadata deserializeVaultMetadata(std::istream& is) {
        VaultMetadata metadata;

        size_t folderCount;
        is.read(reinterpret_cast<char*>(&folderCount), sizeof(folderCount));
        for (size_t i = 0; i < folderCount; ++i)
            metadata.foldersToStore.push_back(deserializePath(is));

        size_t fileCount;
        is.read(reinterpret_cast<char*>(&fileCount), sizeof(fileCount));
        for (size_t i = 0; i < fileCount; ++i) {
            FileInfo file;
            file.filename = deserializeString(is);
            is.read(reinterpret_cast<char*>(&file.file_size), sizeof(file.file_size));
            file.hash = deserializeVectorUChar(is);

            size_t chunkCount;
            is.read(reinterpret_cast<char*>(&chunkCount), sizeof(chunkCount));
            for (size_t j = 0; j < chunkCount; ++j) {
                ChunkInfo chunk;
                is.read(reinterpret_cast<char*>(&chunk.order_index), sizeof(chunk.order_index));
                chunk.chunk_path = deserializePath(is);
                file.chunks.push_back(chunk);
            }

            metadata.files.push_back(file);
        }

        return metadata;
    }
}