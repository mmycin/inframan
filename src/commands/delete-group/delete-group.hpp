#pragma once
#include <string>
#include <vector>

namespace commands {

class DeleteGroup {
public:
    inline static const std::string flag = "dg,--delete-group";
    static const inline std::vector<std::string> flags = {"delete-group", "dg"};

    static void execute();
    void run();

private:
    std::string group_name;
    
    void selectGroup();
    void confirmAndDelete();
};

} // namespace commands
