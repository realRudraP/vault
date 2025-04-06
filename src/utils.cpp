#include "../include/utils.h"
std::string Utilities::takePwdFromUser(const std::string prompt)
{
    std::string password1, password2;
    HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdInput, &mode);

    do {
        std::cout << prompt << ": ";
        SetConsoleMode(hStdInput, mode & (~ENABLE_ECHO_INPUT));
        std::getline(std::cin, password1);
        std::cout << "Confirm password: ";
        std::getline(std::cin, password2);
        SetConsoleMode(hStdInput, mode);
        std::cout << std::endl;

        if (password1 != password2) {
            std::cout << "Passwords do not match. Please try again.\n";
        }
    } while (password1 != password2);

    return password1;
}
std::string Utilities::takePwdOnce(const std::string prompt)
{
    std::string password;
    HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdInput, &mode);

    std::cout << prompt << ": ";
    SetConsoleMode(hStdInput, mode & (~ENABLE_ECHO_INPUT));
    std::getline(std::cin, password);

    return password;
}
void Utilities::securelyClearCharVector(std::vector<char>& buf)
{
    std::fill(buf.begin(), buf.end(), 0);
    buf.clear();
    buf.shrink_to_fit();
}
std::vector<unsigned char> Utilities::generateSalt(size_t length)
{
    std::vector<unsigned char> salt(length);
    if (RAND_bytes(salt.data(), length) != 1) {
        throw std::runtime_error("Failed to generate random salt");
    }
    return salt;
}

std::vector<std::string> Utilities::tokenize(const std::string& inputLine)
{
    std::vector<std::string> tokens;
    std::istringstream tokenStream(inputLine);
    std::string token;
    while (tokenStream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
bool Utilities::getBoolFromUser(const std::string prompt, bool defaultValue)
{
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
int Utilities::getPositiveIntFromUser(const std::string prompt, int defaultValue)
{
    std::string input;
    std::cout << prompt << " (default is " << defaultValue << "): ";
    std::getline(std::cin, input);
    if (input.empty()) {
        return defaultValue;
    } else {
        try {
            int value = std::stoi(input);
            if (value <= 0) {
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

void Utilities::printHex(const std::vector<unsigned char>& data)
{
    for (unsigned char byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    std::cout << std::dec << std::endl;
}
void Utilities::changeDirectory(fs::path path)
{
    if (fs::exists(path)) {
        fs::current_path(path);
    } else {
        std::cerr << "Directory does not exist: " << path << std::endl;
    }
}