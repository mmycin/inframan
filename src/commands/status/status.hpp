#pragma once
#include <string>
#include <vector>

namespace commands {

class Status {
public:
    inline static const std::string flag = "status";
    static const inline std::vector<std::string> flags = {"status", "st"};

    static void execute();
    void run();

private:
    std::string group_name;

    void selectGroup();
    void showAllJobsStatus();
    std::string getJobStatus(const std::string& job_name, 
                             GroupConfig::Providers provider, 
                             GroupConfig::Type type, 
                             const std::string& file_path);
};

} // namespace commands
