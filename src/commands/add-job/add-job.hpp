#pragma once
#include <string>
#include <vector>

namespace commands {

class AddJob {
public:
    inline static const std::string flag = "aj,--add-job";
    static const inline std::vector<std::string> flags = {"add-job", "aj"};

    static void execute();
    void run();

private:
    std::string group_name;
    std::string job_name;
    std::string file_path;
    std::string command;

    void selectGroup();
    void promptFields();
    void validate();
};

} // namespace commands
