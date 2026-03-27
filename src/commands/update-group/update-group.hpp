#pragma once
#include <string>
#include <vector>
#include "group-config.hpp"

namespace commands {

class UpdateGroup {
public:
    inline static const std::string flag = "ug,--update-group";
    static const inline std::vector<std::string> flags = {"update-group", "ug"};

    static void execute();
    void run();

private:
    std::string group_name;
    GroupConfig::Providers provider;
    GroupConfig::Type group_type;

    void selectGroup();
    void promptUpdates();
};

} // namespace commands
