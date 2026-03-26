#pragma once
#include <string>
#include <vector>

namespace commands {

class ReadJob {
public:
    inline static const std::string flag = "rj,--read-jobs";
    static const inline std::vector<std::string> flags = {"read-jobs", "rj"};

    static void execute();
    void run();

private:
    std::string group_name;
    void selectGroup();
};

} // namespace commands
