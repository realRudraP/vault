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
    std::cout << std::endl;

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