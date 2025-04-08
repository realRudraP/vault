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
    INVALID,
    EXIT
};

enum CommandType {
    ENCRYPT,
    DECRYPT,
    CHUNK,
    SDELETE,
    INVALID
};

class Command{
    BaseCommand baseCommand;
    std::optional<std::vector<CommandType>> options;
    std::optional<fs::path> filePath;
    std::optional<std::string> internalName;  
    public:
    Command(BaseCommand base,std::optional<std::vector<CommandType>> opts,std::optional<fs::path> path, std::optional<std::string> internalName){
        this->baseCommand = base;
        options=opts;
        filePath=path;
        this->internalName = internalName;
    }
    Command() { this->baseCommand = BaseCommand::INVALID; };
    static Command makeTrivialCommand(){
        return Command(BaseCommand::TRIVIAL, std::nullopt, std::nullopt, std::nullopt);
    }
};

class Parser{
    public:
        Parser();
        Command parse(std::vector<std::string> args);
};
#endif