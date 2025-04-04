#include "../include/utils.h"
std::string Utilities::takePwdFromUser()
{
    std::string password;
    HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdInput, &mode);
    SetConsoleMode(
        hStdInput,
        mode & (~ENABLE_ECHO_INPUT));
    std::cin.getline(password.data(), 100); 

    SetConsoleMode(hStdInput, mode);
    return password;
}

void Utilities::securelyClearCharVector(std::vector<char>&buf){
    std::fill(buf.begin(), buf.end(), 0);
    buf.clear();
    buf.shrink_to_fit();
}
std::vector<unsigned char> Utilities::generateSalt(size_t length) {
    std::vector<unsigned char> salt(length);
    if (RAND_bytes(salt.data(), length) != 1) {
        throw std::runtime_error("Failed to generate random salt");
    }
    return salt;
}

std::vector<std::string> Utilities::tokenize(const std::string &inputLine)
{
    std::vector<std::string> tokens;
    std::istringstream tokenStream(inputLine);
    std::string token;
    while (tokenStream >> token)
    {
        tokens.push_back(token);
    }
    return tokens;

}
bool Utilities::getBoolFromUser(const std::string prompt, bool defaultValue) {
    std::string input;
    std::cout << prompt << " (Y/n): ";
    std::getline(std::cin, input);
    if (input.empty()) {
        return defaultValue;
    } else if (input == "Y" || input == "y") {
        return true;
    } else if (input == "N" || input == "n") {
        return false;
    } else {
        std::cerr << "Invalid input. Defaulting to " << (defaultValue ? "true" : "false") << "." << std::endl;
        return defaultValue;
    }
}
int Utilities::getPositiveIntFromUser(const std::string prompt, int defaultValue ) {
    std::string input;
    std::cout << prompt << " (default is " << defaultValue << "): ";
    std::getline(std::cin, input);
    if (input.empty()) {
        return defaultValue;
    } else {
        try {
            int value = std::stoi(input);
            if(value <= 0) {
                std::cerr << "Invalid input. Defaulting to " << defaultValue << "." << std::endl;
                return defaultValue;
            } else {
                return value;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid input. Defaulting to " << defaultValue << "." << std::endl;
            return defaultValue;
        }
    }
}