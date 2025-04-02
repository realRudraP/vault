#ifndef VAULT_PARSER_H
#define VAULT_PARSER_H
#include<filesystem>

namespace fs = std::filesystem;
enum CommandType {
    ENCRYPT,
    DECRYPT,
    LIST_FILES,
    HELP
};

class Command{
    CommandType type;
    fs::path filePath;
};

class Parser{
    public:
        Parser();

};
#endif