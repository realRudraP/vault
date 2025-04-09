#ifndef VAULT_PARSER_H
#define VAULT_PARSER_H
#include<filesystem>
#include <vector>
#include<optional>
#include<iostream>
#include "utils.h"
namespace fs = std::filesystem;
enum BaseCommand {
    ADD,
    FETCH,
    HELP,
    SDELETE,
    TRIVIAL,
    ENCRYPT,
    DECRYPT,
    LIST,
    INVALID,
    EXIT
};

class Command{
    public:
    BaseCommand baseCommand;
    bool secureDelete;
    std::optional<fs::path> filePath;
    std::optional<std::string> internalName;  
    Command(BaseCommand base, bool secureDelete, std::optional<fs::path> path, std::optional<std::string> internalName){
        this->baseCommand = base;
        this->secureDelete = secureDelete;
        filePath=path;
        this->internalName = internalName;
    }
    Command() { 
        this->baseCommand = BaseCommand::INVALID;
        this->secureDelete = false;
    };
    static Command makeTrivialCommand(){
        return Command(BaseCommand::TRIVIAL, false, std::nullopt, std::nullopt);
    }
};

class Parser{
    public:
        Parser();
        Command parse(std::vector<std::string> args);
};
#endif