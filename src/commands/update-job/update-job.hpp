#pragma once
#include <string>
#include <vector>

namespace commands {

class UpdateJob {
public:
    inline static const std::string flag = "uj,--update-job";
    static const inline std::vector<std::string> flags = {"update-job", "uj"};

    static void execute();
    void run();

private:
    std::string group_name;
    std::string job_name;
    std::string file_path;
    std::string command;
    
    void selectGroup();
    void selectJob();
    void promptUpdates();
};

} // namespace commands
