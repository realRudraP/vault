#include "../include/parser.h"
#include "../include/manager.h"
/*

    TODO: Move all the trivial functions out
    of the parsing logic

*/
Parser::Parser() { }

Command Parser::parse(std::vector<std::string> args)
{
    if (args.size() == 0) {
        return Command::makeTrivialCommand();
    }
    Command command;
    if (args[0] == "add") {
        if (args.size() < 2) {
            std::cerr << "Add command requires a file path" << std::endl;
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        fs::path inputPath = args[1];
        if (!fs::exists(inputPath)) {
            std::cerr << "File does not exist: " << inputPath << std::endl;
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        if (fs::is_directory(inputPath)) {
            std::cerr << "Vault currently doesn't support adding directories: " << inputPath << std::endl;
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        std::optional<std::string> internalName = std::nullopt;
        auto it = std::find(args.begin() + 2, args.end(), "-name");
        if (it != args.end() && (it + 1) != args.end()) {
            internalName = *(it + 1);
        } else {
            internalName = inputPath.filename().string();
        }
        if (std::find(args.begin() + 2, args.end(), "-sdel") != args.end()) {
            command = Command(BaseCommand::ADD, true, inputPath, internalName);
        } else {
            command = Command(BaseCommand::ADD, false, inputPath, internalName);
        }
        return command;
    } else if (args[0] == "fetch") {
        if(args.size()!=2){
            std::cerr << "Expected fetch <filename>";
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        return Command(BaseCommand::FETCH, false, std::nullopt, args[1]);

    } else if (args[0] == "sdelete") {
        if(args.size()!=2){
            std::cerr<< "Expected syntax: delete <filename/path>";
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        fs::path path(args[1]);
        return Command(BaseCommand::SDELETE, true, path, std::nullopt);

    } else if (args[0] == "help") {
        if (args.size() > 1) {
            std::cout << "Recieved help command. Ignoring rest of the line" << std::endl;
        }
        return Command(BaseCommand::HELP, false, std::nullopt, std::nullopt);
    } else if (args[0] == "exit") {
        if (args.size() > 1) {
            std::cout << "Recieved exit command. Ignoring rest of the line" << std::endl;
        }
        return Command(BaseCommand::EXIT, false, std::nullopt, std::nullopt);
    } else if (args[0] == "ls" || args[0] == "dir") {
        if (args.size() == 2) {
            fs::path path = fs::path(args[1]);
            if (fs::exists(path) && fs::is_directory(path)) {
                for (const auto& entry : fs::directory_iterator(path)) {
                    std::cout << entry.path() << std::endl;
                }
            } else {
                std::cout << "Directory does not exist." << std::endl;
            }
        } else if (args.size() == 1) {
            fs::path currentPath = fs::current_path();
            for (const auto& entry : fs::directory_iterator(currentPath)) {
                std::cout << entry.path() << std::endl;
            }
        } else {
            std::cout << "Usage: ls <directory>" << std::endl;
        }
        return Command::makeTrivialCommand();
    } else if (args[0] == "cd") {
        if (args.size() > 1) {
            fs::path newPath = fs::path(args[1]);
            Utilities::changeDirectory(newPath);
        } else {
            std::cout << "Usage: cd <directory>" << std::endl;
        }
        return Command::makeTrivialCommand();
    }else if(args[0]=="encrypt"){
        if(args.size()<2){
            std::cerr << "Expected filepath after encrypt" << std::endl;
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        fs::path filePath = fs::path(args[1]);
        bool secureDelete = Config::getInstance().enableSecureDeletion;
        std::optional<std::string> internalName = std::nullopt;
        if(fs::exists(filePath)){
            
        auto it = std::find(args.begin() + 2, args.end(), "-name");
        if (it != args.end() && (it + 1) != args.end()) {
            internalName = *(it + 1)+".vaultaes";
        } else {
            internalName = filePath.filename().string()+".vaultaes";
        }
        auto ite = std::find(args.begin() + 2, args.end(), "-sDel");
        if (ite != args.end() && (ite + 1) != args.end()) {
            secureDelete = true;
        }
        }else{
            std::cerr<<"The specified file does not exist. Aborting encryption";
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        return Command(BaseCommand::ENCRYPT, secureDelete, filePath, internalName);
    }else if(args[0]=="decrypt"){
        if(args.size()<2){
            std::cerr << "Expected filepath after decrypt" << std::endl;
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
        fs::path filePath = fs::path(args[1]);
        if(fs::exists(filePath)){
            if(filePath.extension()!=".vaultaes"){
                std::cerr << "You seem to be trying to decrypt a file which was not encrypted by vault.\n Decryption is not possible" << std::endl;
            }
            return Command(BaseCommand::DECRYPT, false, filePath, std::nullopt);
        }else{
            std::cerr << "The given filepath doesn't exist" << std::endl;
            return Command(BaseCommand::INVALID, false, std::nullopt, std::nullopt);
        }
    }else if(args[0]=="list"){
        if(args.size()>1){
            std::cout << "Received list command. Ignoring rest of line" << std::endl;
            
        }
        return Command(BaseCommand::LIST, false, std::nullopt, std::nullopt);
    }
}