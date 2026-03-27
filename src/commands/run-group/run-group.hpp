#pragma once
#include <string>
#include <vector>

namespace commands {

class RunGroup {
public:
    inline static const std::string flag = "run";
    static const inline std::vector<std::string> flags = {"run", "rg"};

    static void execute();
    void run();

private:
    std::string group_name;

    void selectGroup();
    void runAllJobs();
    int runSingleJob(const std::string& job_name,
                     const std::string& command,
                     const std::string& file_path);
};

} // namespace commands
