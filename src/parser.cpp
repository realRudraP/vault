#include "../include/parser.h"

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
            return Command(BaseCommand::INVALID, std::nullopt, std::nullopt, std::nullopt);
        }
        fs::path inputPath = args[1];
        if (!fs::exists(inputPath)) {
            std::cerr << "File does not exist: " << inputPath << std::endl;
            return Command(BaseCommand::INVALID, std::nullopt, std::nullopt, std::nullopt);
        }
        if (fs::is_directory(inputPath)) {
            std::cerr << "Vault currently doesn't support adding directories: " << inputPath << std::endl;
            return Command(BaseCommand::INVALID, std::nullopt, std::nullopt, std::nullopt);
        }
        std::optional<std::string> internalName = std::nullopt;
        auto it = std::find(args.begin() + 2, args.end(), "-name");
        if (it != args.end() && (it + 1) != args.end()) {
            internalName = *(it + 1);
        } else {
            internalName = inputPath.filename().string();
        }
        if (std::find(args.begin() + 2, args.end(), "-sdel") != args.end()) {
            secureDeleteFlag = true;
        }

    } else if (args[0] == "fetch") {

    } else if (args[0] == "delete") {

    } else if (args[0] == "help") {
        if (args.size() > 1) {
            std::cout << "Recieved help command. Ignoring rest of the line" << std::endl;
        }
        return Command(BaseCommand::HELP, std::nullopt, std::nullopt, std::nullopt);
    } else if (args[0] == "exit") {
        if (args.size() > 1) {
            std::cout << "Recieved exit command. Ignoring rest of the line" << std::endl;
        }
        return Command(BaseCommand::EXIT, std::nullopt, std::nullopt, std::nullopt);
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
    }
}