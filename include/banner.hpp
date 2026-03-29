// banner.hpp
#pragma once
#include <string>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
#include "libraries/rang.hpp"

class AsciiBanner {
public:
    static void enableUTF8() {
        #ifdef _WIN32
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
        #endif
    }
    
    static const std::string& getHelloBanner() {
        static const std::string banner = 
        "█████               ██████                                                          \n"
        "░░███               ███░░███                                                         \n"
        " ░███  ████████    ░███ ░░░  ████████   ██████   █████████████    ██████   ████████  \n"
        " ░███ ░░███░░███  ███████   ░░███░░███ ░░░░░███ ░░███░░███░░███  ░░░░░███ ░░███░░███ \n"
        " ░███  ░███ ░███ ░░░███░     ░███ ░░░   ███████  ░███ ░███ ░███   ███████  ░███ ░███ \n"
        " ░███  ░███ ░███   ░███      ░███      ███░░███  ░███ ░███ ░███  ███░░███  ░███ ░███ \n"
        " █████ ████ █████  █████     █████    ░░████████ █████░███ █████░░████████ ████ █████\n"
        "░░░░░ ░░░░ ░░░░░  ░░░░░     ░░░░░      ░░░░░░░░ ░░░░░ ░░░ ░░░░░  ░░░░░░░░ ░░░░ ░░░░░ \n"
        "                                                                                     \n"
        "                                                                                     \n"
        "                                                                                     \n";
        return banner;
    }
    
    static void print() {
        enableUTF8();
        std::cout << rang::fg::cyan << getHelloBanner() << rang::fg::reset << std::endl;
    }
};