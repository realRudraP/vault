#pragma once

#include <iostream>
#include "colors.h"
#ifndef NDEBUG
#define LOG_INFO(msg) std::cout <<Color::BLUE<< "[INFO]: " << msg <<Color::RESET<< std::endl
#define LOG_WARN(msg) std::cout<<Color::YELLOW << "[WARN]: " << msg <<Color::RESET<< std::endl
#define LOG_ERROR(msg) std::cerr <<Color::RED<< "[ERROR]: " << msg << Color::RESET<<std::endl
#else
#define LOG_INFO(msg) void(0)
#define LOG_WARN(msg) void(0)
#define LOG_ERROR(msg) void(0)
#endif