#pragma once
#include <string>
#include <vector>

namespace commands {

class DeleteJob {
public:
    inline static const std::string flag = "dj,--delete-job";
    static const inline std::vector<std::string> flags = {"delete-job", "dj"};

    static void execute();
    void run();

private:
    std::string group_name;
    std::string job_name;
    
    void selectGroup();
    void selectJob();
    void confirmAndDelete();
};

} // namespace commands
