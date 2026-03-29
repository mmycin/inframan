#pragma once
#include <iostream>
#include <string>
#include "app-config.hpp"
#include "rang.hpp"


namespace commands {
    class Version {
    public:
        inline static const std::string flag = "-v,--version";
        
        static void execute() {
            std::cout << rang::style::bold << rang::fg::cyan << AppConfig::name << rang::style::reset 
                      << " v" << rang::fg::yellow << AppConfig::version << rang::fg::reset << std::endl;
        }
    };
}