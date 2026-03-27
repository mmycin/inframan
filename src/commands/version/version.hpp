#pragma once
#include <iostream>
#include <string>
#include "app-config.hpp"


namespace commands {
    class Version {
    public:
        inline static const std::string flag = "-v,--version";
        
        static void execute() {
            std::cout << AppConfig::name << " v" << AppConfig::version << std::endl;
        }
    };
}