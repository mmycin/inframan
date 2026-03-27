#pragma once
#include <string>
#include <vector>

namespace commands {

class UseGroup {
public:
    inline static const std::string flag = "use";
    static const inline std::vector<std::string> flags = {"use", "active"};

    static void execute();
    void run();

private:
    std::string group_name;
    void selectGroup();
};

} // namespace commands
