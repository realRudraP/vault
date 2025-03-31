#include "../include/utils.h"
std::vector<char> Utilities::takePwdFromUser()
{
    std::vector<char> password(50);
    HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdInput, &mode);
    SetConsoleMode(
        hStdInput,
        mode & (~ENABLE_ECHO_INPUT));
    std::cin.getline(password.data(), password.size());
    std::cout << std::endl;

    SetConsoleMode(hStdInput, mode);
    return password;
}

void Utilities::securelyClearCharVector(std::vector<char>&buf){
    std::fill(buf.begin(), buf.end(), 0);
    buf.clear();
    buf.shrink_to_fit();
}