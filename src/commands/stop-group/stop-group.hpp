#pragma once
#include <string>
#include <vector>

namespace commands {

class StopGroup {
public:
    inline static const std::string flag = "stop";
    static const inline std::vector<std::string> flags = {"stop", "sg"};

    static void execute();
    void run();

private:
    std::string group_name;

    void selectGroup();
    void stopAllJobs();
    int stopSingleJob(const std::string& job_name,
                      const std::string& command,
                      const std::string& file_path);
};

} // namespace commands
