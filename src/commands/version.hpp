#pragma once
#include <iostream>
#include <string>
#include "app-config.hpp"

using namespace std;

namespace commands {
    class Version {
    public:
        inline static const string flag = "-v,--version";
        
        static void execute() {
            cout << AppConfig::name << " v" << AppConfig::version << endl;
        }
    };
}