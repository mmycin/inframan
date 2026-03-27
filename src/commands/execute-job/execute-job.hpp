#pragma once
#include <string>
#include <vector>

namespace commands {

class ExecuteJob {
public:
    inline static const std::string flag = "ej";
    static const inline std::vector<std::string> flags = {"ej", "execute"};

    static void execute();
    void run();

private:
    std::string group_name;
    std::string job_name;

    void selectGroup();
    void selectJob();
    void runCommand();
};

} // namespace commands
