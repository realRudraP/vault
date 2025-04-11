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
    OPENSSL_cleanse(password2.data(), password2.length());
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
    SetConsoleMode(hStdInput, mode);
    return password;
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

std::string Utilities::generateUUID(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    auto randHex = [&]() -> char {
        int val = dis(gen);
        return "0123456789abcdef"[val];
    };

    std::stringstream ss;

    for (int i = 0; i < 8; ++i) ss << randHex();
    ss << "-";
    for (int i = 0; i < 4; ++i) ss << randHex();
    ss << "-4"; // UUID version 4
    for (int i = 0; i < 3; ++i) ss << randHex();
    ss << "-";
    ss << "89ab"[dis(gen) % 4]; // variant bits
    for (int i = 0; i < 3; ++i) ss << randHex();
    ss << "-";
    for (int i = 0; i < 12; ++i) ss << randHex();

    return ss.str();
}

bool Utilities::deleteFile(fs::path path,bool deleteSecurely,size_t numberOfPasses){
    if (deleteSecurely) {
        try {
            if (!fs::exists(path) || !fs::is_regular_file(path)) {
                std::cerr << "Secure deletion is only supported for regular files. Skipping: " << path << std::endl;
                return false;
            }
    
            std::error_code ec;
            std::uintmax_t fileSize = fs::file_size(path, ec);
            if (ec || fileSize == 0) {
                std::cerr << "Error getting file size or file is empty: " << ec.message() << std::endl;
                return false;
            }
    
            for (size_t pass = 0; pass < numberOfPasses; ++pass) {
                std::vector<unsigned char> randomData(fileSize);
                if (RAND_bytes(randomData.data(), static_cast<int>(fileSize)) != 1) {
                    std::cerr << "Failed to generate cryptographically secure random data.\n";
                    return false;
                }
    
                HANDLE hFile = CreateFileW(
                    path.wstring().c_str(),
                    GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL
                );
    
                if (hFile == INVALID_HANDLE_VALUE) {
                    std::cerr << "Failed to open file for secure overwrite: " << path << std::endl;
                    return false;
                }
    
                DWORD bytesWritten;
                if (!WriteFile(hFile, randomData.data(), static_cast<DWORD>(fileSize), &bytesWritten, NULL) || bytesWritten != fileSize) {
                    std::cerr << "Failed to write random data to file: " << path << std::endl;
                    CloseHandle(hFile);
                    return false;
                }
    
                if (!FlushFileBuffers(hFile)) {
                    std::cerr << "Failed to flush file buffers for: " << path << std::endl;
                }
    
                CloseHandle(hFile);
                OPENSSL_cleanse(randomData.data(), randomData.size());
            }
    
            return fs::remove(path);
        } catch (const std::exception& e) {
            std::cerr << "Secure deletion failed for " << path << " : " << e.what() << std::endl;
            return false;
        }
    }
    else{
        try{
            if(!fs::exists(path)){
                std::cerr << "Deletion failed. Path " << path.string() << " was not found";
                return false;
            }
            if(fs::is_directory(path)){
                for(const auto& entry:fs::directory_iterator(path)){
                    Utilities::deleteFile(entry.path(), false, 1); // Recursive call to empty directories
                }
                fs::remove(path);
                return true;
            }else{
                fs::remove(path);
                return true;
            }
        }catch(const std::exception& e){
            std::cerr << "Error in deleting file " << path.string() << " : " << e.what() << std::endl;
            return false;
        }
    }
}